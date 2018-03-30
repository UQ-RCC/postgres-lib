#include "database_io.h"

MeshMakerDB::MeshMakerDB(ConnectParameters params)
{
	
	meshConn = PQsetdbLogin(
		params.host.c_str(),
		params.port.c_str(), 
		NULL, 
		NULL,
		params.instance.c_str(),
		params.userName.c_str(),
		params.password.c_str());

	ConnStatusType status = PQstatus(meshConn);
	if (status != CONNECTION_OK)
	{
		throw std::runtime_error("no database connection");
	}

	frameConn = PQsetdbLogin(
		params.host.c_str(),
		params.port.c_str(),
		NULL,
		NULL,
		params.instance.c_str(),
		params.userName.c_str(),
		params.password.c_str());
}

MeshMakerDB::~MeshMakerDB()
{
	PQfinish(meshConn);
	PQfinish(frameConn);
}

class SqlBuilder
{
	
public:

	SqlBuilder(const std::string& function) : function(function) {}

	SqlBuilder& operator<<(const std::string & param)
	{	
		length.push_back(param.length());
		format.push_back(0);
		parameter.push_back(param.c_str());
		parameterDef.push_back(std::string().append("$").append(std::to_string(parameter.size())));
		return *this;
	}

	SqlBuilder& operator<<(const int param)
	{		
		length.push_back(sizeof(int));
		format.push_back(1);
		intKeeper.push_back(htonl(param));
		parameter.push_back((char *) &intKeeper.back());
		parameterDef.push_back(std::string().append("$").append(std::to_string(parameter.size())).append("::integer"));
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

	const char ** getParameter()
	{
		return NumParameters() ? parameter.data() : NULL;
	}

	int NumParameters()
	{
		return parameter.size();
	}

	PGresult * execute(PGconn * meshConn)
	{
		return PQexecParams(meshConn, getSQL().c_str(), NumParameters(), NULL, getParameter(), GetParamLength(), GetParamFormat(), 0);
	}

	//This is how doubles are done (for future ref)	
	void to_nbo(const double in, double * out)
	{
		uint64_t * i = (uint64_t *)&in;
		uint32_t * r = (uint32_t *)out;

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
	std::deque<int> intKeeper;
};

/*
int MeshMakerDB::InsertMesh(const Mesh mesh)
{
	boost::lock_guard<boost::mutex> lock(meshConnMtx);
	cout << "inserting mesh" << mesh.frameId << endl;
	SqlBuilder sqb("insert_gl_frame");
	sqb << mesh.frameId << mesh.objectCount << mesh.cellCount << mesh.status;	
	PGresult * meshRS = sqb.execute(meshConn);
	if (!checkStmt(meshRS, meshConn))
	{
		printf("Insert mesh failed with code %s\n", mesh.processCode.c_str());
	}
	PQclear(meshRS);	
	return mesh.frameId;
}
*/

void MeshMakerDB::TestConnection()
{
	boost::lock_guard<boost::mutex> lock(meshConnMtx);
	fmt::print("testing");
	PGresult * testRS = PQexec(meshConn, "select version()");
	if (checkStmt(testRS, meshConn))
	{
		if (!PQgetisnull(testRS, 0, 0))
		{
			fmt::print("{}\n", PQgetvalue(testRS, 0, 0));
		}
		else
		{
			fmt::print("no results\n");
		}
	}
	PQclear(testRS);
}

int MeshMakerDB::StartLog()
{
	SqlBuilder sqb("start_log");
	boost::unique_lock<boost::mutex> lock(meshConnMtx);	
	PGresult * logRS = sqb.execute(meshConn);
	if (checkStmt(logRS, meshConn))
	{
		if (!PQgetisnull(logRS, 0, 0))
		{
			return atoi(PQgetvalue(logRS, 0, 0));
		}
	}
	else
	{
		return 0;
	}
	PQclear(logRS);
}

void MeshMakerDB::EndLog(int logID, const std::string & type, int foreignID, const std::string & message)
{
	SqlBuilder sqb("end_log");
	sqb << logID << type << foreignID << message;	
	boost::unique_lock<boost::mutex> lock(meshConnMtx);
	PGresult * logRS = sqb.execute(meshConn);
	if (!checkStmt(logRS, meshConn))
	{
		printf("insert into end log failed\n");
	}
	PQclear(logRS);	
}

bool MeshMakerDB::checkStmt(PGresult * result, PGconn * conn)
{
	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		fprintf(stderr, "SLQ error: %s\n", PQerrorMessage(conn));		
		fprintf(stderr, "State: %s\n", PQresultErrorField(result, PG_DIAG_SQLSTATE));
		fprintf(stderr, "Primary: %s\n", PQresultErrorField(result, PG_DIAG_MESSAGE_PRIMARY));
		fprintf(stderr, "Detail: %s\n", PQresultErrorField(result, PG_DIAG_MESSAGE_DETAIL));
		fprintf(stderr, "Pos: %s\n", PQresultErrorField(result, PG_DIAG_STATEMENT_POSITION));
		fprintf(stderr, "Context: %s\n", PQresultErrorField(result, PG_DIAG_CONTEXT));		
		return false;
	}
	return true;
}
