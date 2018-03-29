// postgres-lib.cpp : Defines the entry point for the application.
//

#include "postgres-lib.h"
#include "MeshMakerDB.h"
#include <fmt/printf.h>

using namespace std;

void foo()
{
	cout << "foooit" << endl;
}


void testDB()
{
	
	ConnectParameters cp;
	cp.host = "localhost";
	cp.port = "5432";
	cp.instance = "phoebe";
	cp.userName = "phoebeadmin";
	cp.password = "password";

	fmt::print("some struct stuff {}\n", cp.host);
	MeshMakerDB db(cp);
	db.TestConnection();
}

void testFMT()
{
	fmt::print("here is some fmt action\n");
}
