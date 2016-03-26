#include<iostream>
#include<stdio.h>
#include "sql_connect.h"
#include<stdlib.h>
#include<string.h>


int main()
{
	char query_string[512];
	char method[512];
	char post_data[512];
	memset(post_data,'\0',sizeof(post_data));
	memset(query_string,'\0',sizeof(query_string));
	memset(method,'\0',sizeof(method));
	std::string user="root";
	std::string passwd="root";
	std::string db="bit_study";
	std::string host="127.0.0.1";
	char name[64];
	char age[12];
	char id[64];
	char hobby[64];
	int content_length=-1;
	char *p=NULL;
	std::string data;
	//printf("<html>\n");
//	printf("<body>\n");
	std::cout<<"<html>"<<std::endl;
	std::cout<<"<body>"<<std::endl;
	std::cout<<"<p>stduent info<br/>"<<std::endl;
	//printf("<p>hello world<br/>\n");
	//char post[512]="data1=2&data2=3";
	//add(post);
	strcpy(method,getenv("METHOD"));
	//printf("<p>method:%s<br/>",method);
	//content_length=atoi(getenv("Content-Length"));
	//printf("<p>content-length:%d<br/>\n",content_length);
	if(strcasecmp("GET",method)==0)
	{
		strcpy(query_string,getenv("QUERY_STRING"));
		printf("<p>method:%s<br/>",method);
		printf("<p>query_string:%s<br/>",query_string);
		p=query_string;
	}
	else if(strcasecmp("POST",method)==0)
	{
	//	printf("hello post\n");
		content_length=atoi(getenv("Content_Length"));
		int i=0;
	//	printf("content_length_cgi:%d\n",content_length);
		for(;i<content_length;++i)
		{
			//printf("read\n");
			read(0,&post_data[i],1);
		}
		post_data[i]='\0';
	//	printf("post_data:%s\n",post_data);
		p=post_data;
	}
	
	while(*p!='\0')
	{
		if(*p=='&' || *p=='=')
			*p=' ';
		p++;
	}
	//printf("post_data:%s\n",post_data);
	sscanf(post_data,"%*s %s %*s %s %*s %s %*s %s",name,age,id,hobby);
	//printf("post_data:%s\n",post_data);
	//printf("name:%s %s %s %s\n",name,age,id,hobby);
	sql_conner conn(user,passwd,db,host);
	conn.begin_connect();
	data+="'";
	data+=name;
	data+="',";
	data+=age;
	data+=",";
	data+=id;
	data+=",";
	data+="'";
	data+=hobby;
	data+="'";
//	std::cout<<data<<std::endl;
	conn.sql_insert(data);
	std::string _sql_data[102][4];
	std::string header[4];
	int curr_row=-1;

	conn.sql_select(header,_sql_data,curr_row);
	
	std::cout<<"<table border=\"1\">"<<std::endl;
	std::cout<<"<tr>"<<std::endl;
	for(int i = 0; i<4; i++){
                std::cout<<"<th>"<<header[i]<<"</th>"<<std::endl;
        }
        std::cout<<"</tr>"<<std::endl;

        for(int i = 0; i<curr_row; i++){
		std::cout<<"<tr>"<<std::endl;
                for(int j=0; j<4; j++){
                        std::cout<<"<td>"<<_sql_data[i][j]<<"</td>"<<std::endl;
                }
                std::cout<<"</tr>"<<std::endl;
        }
	std::cout<<"</table>"<<std::endl;
	std::cout<<"</body>"<<std::endl;
	std::cout<<"</html>"<<std::endl;
	return 0;
}
