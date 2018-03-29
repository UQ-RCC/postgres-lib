#pragma once

#include <libpq-fe.h>
#include <deque>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <nlohmann/json.hpp>
#include <fmt/printf.h>

struct ConnectParameters
{
	std::string host;
	std::string port;
	std::string instance;
	std::string userName;
	std::string password;
};

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