#pragma once

#include <iostream>
#include <database_lib.h>

int main()
{
	
	ConnectParameters cp;
	cp.host = "localhost";
	cp.port = "5432";
	cp.instance = "phoebe";
	cp.userName = "phoebeadmin";
	cp.password = "password";
	
	DBExecutor executor(cp);
	executor.execute_procedure("q_log", "test from outside", "{}");
	//executor.execute();

	// testDB(cp);

	std::cin.get();
	return 0;
}
