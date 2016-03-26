#include"mysql.h"
#include<iostream>
#include<string>

class sql_conner
{
public:
	bool begin_connect();
	bool close_connect();
	bool sql_insert(std::string data); 
	bool sql_select(std::string field_name[],std::string _out_data[][4],int &_out_row);
public:
	sql_conner(std::string &user,std::string &passwd,std::string &db,std::string &host);
	~sql_conner();
private:
	
	MYSQL_RES * res;
	MYSQL *mysql_conn;
	std::string _user;
	std::string _passwd;
	std::string _db;
	std::string _host;
};
