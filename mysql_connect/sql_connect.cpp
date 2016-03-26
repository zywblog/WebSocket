#include"sql_connect.h"

sql_conner::sql_conner(std::string &user,std::string &passwd,std::string &db,std::string &host)
{
	this->mysql_conn=mysql_init(NULL);
	this->res=NULL;
	this->_user=user;
	this->_passwd=passwd;
	this->_db=db;
	this->_host=host;
}
sql_conner::~sql_conner()
{
	close_connect();
}
bool sql_conner::begin_connect()
{
	std::cout<<"<p>begin connect</p>"<<std::endl;
	if(mysql_real_connect(mysql_conn,_host.c_str(),_user.c_str(),_passwd.c_str(),_db.c_str(),3306,NULL,0)==NULL)
	{
		//std::cout<<"mysql_real_conner error"<<std::endl;
		return false;
	}
	else
	{
		//std::cout<<"mysql connect success"<<std::endl;
		return true;
	}
}
bool sql_conner::sql_select(std::string field_name[],std::string _out_data[][4],int &_out_row)
{
	std::string sql="select * from project ";
	if(mysql_query(mysql_conn,sql.c_str())==0)
	{
		//std::cout<<"<p>query success</p>"<<std::endl;
	}
	else
	{
		//std::cout<<"<p>query failed</p>"<<std::endl;
	}
	res=mysql_store_result(mysql_conn);
	int row_num=mysql_num_rows(res);
	int field_num=mysql_num_fields(res);
	_out_row=row_num;
	MYSQL_FIELD *fd = NULL;
	int i=0;
	for(;fd=mysql_fetch_field(res);)
	{
	//	std::cout<<fd->name<<"\t";
		field_name[i++]=fd->name;
	}
	for(i=0;i<row_num;i++)
	{
		MYSQL_ROW _row = mysql_fetch_row(res);
		if(_row)
		{
			int j=0;
			for(;j<field_num;++j)
			{
				_out_data[i][j]=_row[j];
				//std::cout<<_row[j]<<"\t";
			}
		}
	}
	return true;
}
bool sql_conner::close_connect()
{
	mysql_close(mysql_conn);
	return true;
}

bool sql_conner::sql_insert(std::string data)
{
	std::string str;
	str+="INSERT INTO project (name,age,id,hobby) values ";
	str+="(";
	str+=data;
	str+=")";
	std::cout<<"insert "<<std::endl;
	if( mysql_query(mysql_conn,str.c_str())==0)
	{	
		//std::cout<<"<p>insert success</p>"<<std::endl;
		return true;
	}
	else
	{
	//	std::cout<<"<p>insert failed</p>"<<std::endl;
		return false;
	}
}
//int main()
//{
//	
//	std::string _sql_data[1024][4];
//	std::string header[4];
//	int curr_row = -1;
//	std::string user="root";
//	std::string passwd="root";
//	std::string db="bit_study";
//	std::string host="127.0.0.1";
//	std::string data="'noble',22,131003,'playing'";
//	sql_conner conn(user,passwd,db,host);	
//	conn.begin_connect();
//	//conn.sql_insert(data);
//	conn.sql_select(header,_sql_data, curr_row);
//	sleep(1);
//	for(int i = 0; i<4; i++){
//		std::cout<<header[i]<<"\t";
//	}
//	std::cout<<std::endl;
//
//	for(int i = 0; i<curr_row; i++){
//		for(int j=0; j<4; j++){
//			std::cout<<_sql_data[i][j]<<"\t";
//		}
//		std::cout<<std::endl;
//	}
//	return 0;
//	
//}
