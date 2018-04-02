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
	auto id = executor.execute_procedure("q_log", "test from outside", "{}");
	std::cout << "query retured: " << id << std::endl;
	std::cin.get();
	return 0;
}

/*
db.execute_procedure(
"insert_experiment",
"test_name",
"test_directory_2",
100,
200,
150,
5,
99,
1,
1,
1.65423423
);

db.execute_procedure("q_log", "test", "{}");
*/