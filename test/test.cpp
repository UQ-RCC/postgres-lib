#include <iostream>
#include <postgres-lib.h>

int main()
{
	/*
	ConnectParameters cp
	{
		"localhost",
		"1252",
		"phoebeadmin",
		"password"
	};
	MeshMakerDB db(cp);
	*/
	std::cout << "Hello CMake." << std::endl;
	foo();
	testDB();
	testFMT();
	
	std::cin.get();
	return 0;
}
