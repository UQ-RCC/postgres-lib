#include <iostream>
#include <postgres_lib.h>

int main()
{
	
	ConnectParameters cp;
	cp.host = "localhost";
	cp.port = "5432";
	cp.instance = "phoebe";
	cp.userName = "phoebeadmin";
	cp.password = "password";
	
	testDB(cp);
	
	std::cin.get();
	return 0;
}
