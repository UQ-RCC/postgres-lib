#pragma once

#include <libpq-fe.h>
#include <deque>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <nlohmann/json.hpp>
#include <fmt/printf.h>
#include <database_lib.h>

class MeshMakerDB
{
private:
	MeshMakerDB();
	bool checkStmt(PGresult * result, PGconn * conn);
	PGconn * meshConn;
	PGconn * frameConn;	
	boost::mutex meshConnMtx;
	boost::mutex frameConnMtx;

public:
	MeshMakerDB(const ConnectParameters & params);
	~MeshMakerDB();
	void test_connection();
	int StartLog();
	void EndLog(int logID, const std::string & type, int foreignID, const std::string & message);	
	int execute_sqb(SqlBuilder & sqb);	
	int execute();
	
};