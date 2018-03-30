#include "postgres_lib.h"
#include "database_io.h"

using namespace std;

void testDB(const ConnectParameters & cp)
{	
	MeshMakerDB db(cp);
	db.test_connection();
	
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
	
	// db.execute_procedure("q_log", "test_type", "{}");

}
