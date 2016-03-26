#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void add(char *string)
{
	printf("hi :)\n");
	int val1,val2;
	char *data1=NULL;
	char *data2=NULL;
	char *str=string;
	while(*str!='\0')
	{
		if(*str=='&')
		{
			*str='\0';
			str++;
		}
		if(*str=='='&& data1==NULL)
		{
			str++;
			data1=str;
		}
		else if(*str=='='&& data1!=NULL)
		{
			str++;
			data2=str;
		}
		else
			str++;
	}
	val1=atoi(data1);
	val2=atoi(data2);
	printf("<p>data1:%s<br/>\n",data1);
	printf("<p>data2:%s<br/>\n",data2);
	printf("<p>sum:%d<br/>\n",val1+val2);
	return ;
}


int main()
{
	char query_string[512];
	char method[512];
	char post_data[512];
	memset(post_data,'\0',sizeof(post_data));
	memset(query_string,'\0',sizeof(query_string));
	memset(method,'\0',sizeof(method));
	
	int content_length=-1;
	printf("<html>\n");
	printf("<body>\n");
	printf("<p>hello world<br/>\n");
	printf("<p>hello world<br/>\n");
	//char post[512]="data1=2&data2=3";
	//add(post);
	strcpy(method,getenv("METHOD"));
	printf("<p>method:%s<br/>",method);
	//content_length=atoi(getenv("Content-Length"));
	//printf("<p>content-length:%d<br/>\n",content_length);
	char *p=query_string;
	if(strcasecmp("GET",method)==0)
	{
		strcpy(query_string,getenv("QUERY_STRING"));
		printf("<p>method:%s<br/>",method);
		printf("<p>query_string:%s<br/>",query_string);
	}
	else if(strcasecmp("POST",method)==0)
	{
		printf("hello post\n");
		content_length=atoi(getenv("Content_Length"));
		int i=0;
		printf("content_length_cgi:%d\n",content_length);
		for(;i<content_length;++i)
		{
			//printf("read\n");
			read(0,&post_data[i],1);
		}
		post_data[i]='\0';
		printf("post_data:%s\n",post_data);
		add(post_data);
	}
	printf("</body>\n");
	printf("</html>\n");
	return 0;
}
