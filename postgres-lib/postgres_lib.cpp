#include "postgres_lib.h"
#include "database_io.h"

using namespace std;

void testDB(const ConnectParameters & cp)
{	
	MeshMakerDB db(cp);
	db.TestConnection();
}
