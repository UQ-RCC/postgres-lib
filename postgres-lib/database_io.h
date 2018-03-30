#pragma once

#include <libpq-fe.h>
#include <deque>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <nlohmann/json.hpp>
#include <fmt/printf.h>
#include <postgres_lib.h>

class SqlBuilder
{

public:

	SqlBuilder(const std::string & function) : function(function) {}

	SqlBuilder& operator<<(const std::string & param)
	{		
		length.push_back(param.length());
		format.push_back(0);
		stringKeeper.push_back(param);		
		parameter.push_back(stringKeeper.back().c_str());
		parameterDef.push_back(std::string().append("$").append(std::to_string(parameter.size())));
		return *this;
	}

	SqlBuilder& operator<<(const int param)
	{
		fmt::print("<< {}\n", param);
		length.push_back(sizeof(int));
		format.push_back(1);		
		intKeeper.push_back(htonl(param));
		parameter.push_back((char *)&intKeeper.back());
		parameterDef.push_back(std::string().append("$").append(std::to_string(parameter.size())).append("::integer"));
		return *this;
	}

	SqlBuilder& operator<<(const double param)
	{
		fmt::print("<< double {}\n", param);
		length.push_back(sizeof(double));
		format.push_back(1);
		doubleKeeper.push_back(0.0);
		to_nbo(param, &doubleKeeper.back());
		parameter.push_back((char *)&doubleKeeper.back());
		parameterDef.push_back(std::string().append("$").append(std::to_string(parameter.size())).append("::double precision"));
		return *this;
	}

	std::string getSQL()
	{
		std::string sql = "select * from ";
		sql.append(function).append("(");
		for (int i = 0; i < NumParameters() - 1; i++)
		{
			sql.append(parameterDef[i]);
			sql.append(", ");
		}
		if (NumParameters() > 0)
		{
			sql.append(parameterDef[NumParameters() - 1]);
		}
		sql.append(")");
		return sql;
	}

	int * GetParamLength()
	{
		return NumParameters() ? length.data() : NULL;
	}

	int * GetParamFormat()
	{
		return NumParameters() ? format.data() : NULL;
	}

	const char ** GetParameter()
	{
		return NumParameters() ? parameter.data() : NULL;
	}

	int NumParameters()
	{
		return parameter.size();
	}

	PGresult * execute(PGconn * meshConn)
	{
		return PQexecParams(meshConn, getSQL().c_str(), NumParameters(), NULL, GetParameter(), GetParamLength(), GetParamFormat(), 0);
	}
		
	void to_nbo(const double in, double * out)
	{
		uint64_t * i = (uint64_t *) &in;
		uint32_t * r = (uint32_t *) out;

		/* convert input to network byte order */
		r[0] = htonl((uint32_t)((*i) >> 32));
		r[1] = htonl((uint32_t)*i);
	}

private:
	std::string function;
	std::vector<int> length;
	std::vector<int> format;
	std::vector<const char *> parameter;
	std::vector<std::string> parameterDef;

	// store a copies of data so that we can push pointers.
	std::deque<int> intKeeper;
	std::deque<double> doubleKeeper;
	std::deque<std::string> stringKeeper;	
};


class MeshMakerDB
{
private:	
	bool checkStmt(PGresult * result, PGconn * conn);
	PGconn * meshConn;
	PGconn * frameConn;	
	boost::mutex meshConnMtx;
	boost::mutex frameConnMtx;
	void execute_sqb(SqlBuilder & sqb);

	SqlBuilder & build_sql(SqlBuilder & sqb)
	{	
		return sqb;
	}

	template<typename T, typename... Args>
	SqlBuilder & build_sql(SqlBuilder & sqb, const T & p, const Args &... rest)
	{
		sqb << p;
		return build_sql(sqb, rest...);
	}

public:
	MeshMakerDB(ConnectParameters params);
	~MeshMakerDB();
	void test_connection();
	int StartLog();
	void EndLog(int logID, const std::string & type, int foreignID, const std::string & message);	
	template<typename... T>
	void execute_procedure(const std::string & procedure, const T &... p)
	{
		SqlBuilder sqb(procedure);
		build_sql(sqb, p...);		
		execute_sqb(sqb);
		//sqb.execute(meshConn);
	}
	
};