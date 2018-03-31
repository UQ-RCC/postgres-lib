#include "database_lib.h"
#include "database_io.h"

using namespace std;

void testDB(const ConnectParameters & cp)
{	
	MeshMakerDB db(cp);
	db.test_connection();
	
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
}

struct DBExecutor::DBExecutorImpl
{
public:
	DBExecutorImpl(const ConnectParameters & cp) : db(cp) {};
	~DBExecutorImpl() {};
	int execute_procedure(SqlBuilder & sqb)
	{
		return db.execute_sqb(sqb);		
	}

private:
	MeshMakerDB db;
};

DBExecutor::DBExecutor(const ConnectParameters & cp) : impl(new DBExecutorImpl(cp)) {}
DBExecutor::~DBExecutor() {}

int DBExecutor::execute_procedure(SqlBuilder & sqb)
{
	return impl->execute_procedure(sqb);
}




