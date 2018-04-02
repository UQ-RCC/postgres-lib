#include "database_io.h"

MeshMakerDB::MeshMakerDB(const ConnectParameters & params)
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

void MeshMakerDB::test_connection()
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

int MeshMakerDB::execute_sqb(SqlBuilder & sqb)
{	
	PGresult * testRS = PQexecParams(meshConn, sqb.getSQL().c_str(), sqb.NumParameters(), NULL, sqb.GetParameter(), sqb.GetParamLength(), sqb.GetParamFormat(), 0);
	if (checkStmt(testRS, meshConn))
	{
		if (!PQgetisnull(testRS, 0, 0))
		{
			return atoi(PQgetvalue(testRS, 0, 0));
		}
		else
		{
			return -1;
		}
	}
	PQclear(testRS);

}

int MeshMakerDB::StartLog()
{
	SqlBuilder sqb("start_log");
	boost::unique_lock<boost::mutex> lock(meshConnMtx);	
	PGresult * logRS = PQexecParams(meshConn, sqb.getSQL().c_str(), sqb.NumParameters(), NULL, sqb.GetParameter(), sqb.GetParamLength(), sqb.GetParamFormat(), 0);
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
	PGresult * logRS = PQexecParams(meshConn, sqb.getSQL().c_str(), sqb.NumParameters(), NULL, sqb.GetParameter(), sqb.GetParamLength(), sqb.GetParamFormat(), 0);
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

int MeshMakerDB::execute()
{
	fmt::print("from derived class...\n");
	return 0;
}
