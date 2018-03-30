#pragma once

#include <libpq-fe.h>
#include <deque>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <nlohmann/json.hpp>
#include <fmt/printf.h>
#include <postgres_lib.h>

class MeshMakerDB
{
private:	
	bool checkStmt(PGresult * result, PGconn * conn);
	PGconn * meshConn;
	PGconn * frameConn;	
	boost::mutex meshConnMtx;
	boost::mutex frameConnMtx;	
public:
	MeshMakerDB(ConnectParameters params);
	~MeshMakerDB();
	void TestConnection();
	int StartLog();
	void EndLog(int logID, const std::string & type, int foreignID, const std::string & message);
};