#include "sql_builder.h"
#include <string>
#include <iostream>
#include <fmt/printf.h>

SqlBuilder::SqlBuilder(const std::string & function) : function(function) {}

SqlBuilder& SqlBuilder::operator<<(const std::string & param)
{
	length.push_back(param.length());
	format.push_back(0);
	stringKeeper.push_back(param);
	parameter.push_back(stringKeeper.back().c_str());
	parameterDef.push_back(std::string().append("$").append(std::to_string(parameter.size())));
	return *this;
}

SqlBuilder& SqlBuilder::operator<<(const int param)
{
	length.push_back(sizeof(int));
	format.push_back(1);
	intKeeper.push_back(mhtonl(param));
	parameter.push_back((char *)&intKeeper.back());
	parameterDef.push_back(std::string().append("$").append(std::to_string(parameter.size())).append("::integer"));
	return *this;
}

SqlBuilder& SqlBuilder::operator<<(const double param)
{
	length.push_back(sizeof(double));
	format.push_back(1);
	doubleKeeper.push_back(0.0);
	to_nbo(param, &doubleKeeper.back());
	parameter.push_back((char *)&doubleKeeper.back());
	parameterDef.push_back(std::string().append("$").append(std::to_string(parameter.size())).append("::double precision"));
	return *this;
}

std::string SqlBuilder::getSQL()
{
	
	fmt::print("building function {} with {} parameters\n", function, NumParameters());
	std::cin.get();
	std::string sql = "select * from ";
	sql.append(function).append("(");
	for (int i = 0; i < (NumParameters() - 1); i++)
	{
		size_t nP = NumParameters() - 1;
		fmt::print("size_t stuff up? {}\n", nP);
		std::cin.get();
		sql.append(parameterDef[i]);
		sql.append(", ");
	}
	if (NumParameters() > 0)
	{
		sql.append(parameterDef[NumParameters() - 1]);
	}
	sql.append(")");
	std::cout << "building... " << sql << std::endl;
	return sql;
}

size_t * SqlBuilder::GetParamLength()
{
	return NumParameters() ? length.data() : NULL;
}

int * SqlBuilder::GetParamFormat()
{
	return NumParameters() ? format.data() : NULL;
}

const char ** SqlBuilder::GetParameter()
{
	return NumParameters() ? parameter.data() : NULL;
}

int SqlBuilder::NumParameters()
{
	return parameter.size();
}

void SqlBuilder::to_nbo(const double in, double * out)
{
	uint64_t * i = (uint64_t *)&in;
	uint32_t * r = (uint32_t *)out;

	/* convert input to network byte order */
	//r[0] = mhtonl((uint32_t)((*i) >> 32));
	//r[1] = mhtonl((uint32_t)*i);
}

int SqlBuilder::mhtonl(int p)
{
	return p;
}