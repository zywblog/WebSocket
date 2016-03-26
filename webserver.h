#include<iostream>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>
#include<pthread.h>
//#include<thread>
#include<sys/stat.h>
using namespace std;

class webserver
{
public:
    webserver()
    {
	httpd=0;
    }
    ~webserver()
    {}
    	int web_init(u_short port)
	{
		struct sockaddr_in seraddr;
		httpd=socket(AF_INET,SOCK_STREAM,0);
		if(httpd<0)
		{
			perror("webserver socket error");
			exit(1);
		}
		int optval=5;
		if(setsockopt(httpd,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof(optval))<0)
		{
			perror("setsockopt error");
			return 1;
		}
		seraddr.sin_family=AF_INET;
		seraddr.sin_port=htons(port);
		seraddr.sin_addr.s_addr=htonl(INADDR_ANY);
		if(bind(httpd,(struct sockaddr*)&seraddr,sizeof(seraddr))==-1)
		{
			perror("bind error");
			exit(1);
		}
		if(port==0)
		{
			socklen_t len=sizeof(seraddr);
			if(getsockname(httpd,(struct sockaddr*)&seraddr,&len)==-1)
			{
				perror(" getsockname error");
				exit(1);
			}
			port=ntohs(seraddr.sin_port);
		}
		if(listen(httpd,10)<0)
		{
			perror("listen error");
			exit(1);
		}
		cout<<port<<endl;
		return port;
	}
	int web_accept()
	{
		//printf("web_accept\n");
		int conn_fd;
		struct sockaddr_in cli_addr;
		socklen_t len=sizeof(cli_addr);
		conn_fd=accept(httpd,(struct sockaddr*)&cli_addr,&len);
		if(conn_fd<0)
		{
			perror("accept error");
			exit(1);
		}
		cout<<"accept sucess"<<endl;
		cout<<"connect IP:"<<inet_ntoa(cli_addr.sin_addr)<<" port :"<<ntohs(cli_addr.sin_port)<<endl;
		return conn_fd;
	}
	int exe_cgi(int conn_fd,char *query_string,char *path,char *method)
	{
		char query_env[512];
		char method_env[512];
		memset(query_env,'\0',sizeof(query_env));
		memset(method_env,'\0',sizeof(method_env));
		char buf[1024];
		memset(buf,'\0',sizeof(buf));
		pid_t pid;
		int size=1;
		int content_length=-1;
    		int status;
    		int cgi_output[2];
    		int cgi_input[2];
		int i=0;
		char c='\0';

    		if(pipe(cgi_output)<0)
    		{
        		page_500(conn_fd);
        		return 0;
    		}
    		if(pipe(cgi_input)<0)
   		{
        		page_500(conn_fd);
        		return 0;
    		}
		printf("exe_cgi method:%s\n",method);
		if(strcasecmp("GET",method)==0)
		{
//			printf("GET\n");
			while( size>0 && strcmp("\n",buf))
				size=get_line(conn_fd,buf,sizeof(buf));
		}
		else if(strcasecmp("POST",method)==0)
		{
//			printf("post cgi\n");
			memset(buf,'\0',sizeof(buf));
			size=get_line(conn_fd,buf,sizeof(buf));
			while( size>0 && strcmp("\n",buf))
			{
				if( strncasecmp(buf,"Content-Length:",strlen("Content-Length:"))==0)
				{
					content_length=atoi(&(buf[16]));
				}
				printf("content_length:%d\n",content_length);	
				size=get_line(conn_fd,buf,sizeof(buf));
			}
			if(content_length==-1)
				page_404(conn_fd);
		}
		printf("send\n");
		memset(buf,'\0',sizeof(buf));
		strcpy(buf,"http/1.1 ");
		strcat(buf,"200 OK \r\n\r\n");
		send(conn_fd,buf,strlen(buf),0);
    		if((pid=fork())<0)
    		{
        		page_500(conn_fd);
        		return 0;
    		}
    		else if(pid==0)//child
    		{
        		if(dup2(cgi_output[1],1)<0)//cgi的输出端绑定文件描述符为1的输出端
			{
				printf("dup2 error\n");
			}
        		if(dup2(cgi_input[0],0)<0)	
				printf("dup2 error\n");
        		close(cgi_output[0]);
        		close(cgi_input[1]);
			
			sprintf(method_env,"METHOD=%s",method);
			if(putenv(method_env)!=0)
			{
				printf("putenv error\n");
				return 0;
			}
//			printf("child method:%s\n",method_env);
//			printf("method_env:%s\n",getenv("METHOD"));
			if(strcasecmp(method,"get")==0)
			{
        			sprintf(query_env,"QUERY_STRING=%s",query_string);
        			if(putenv(query_env)!=0)
				{
					printf("putenv error\n");
					return 0;
				}
//				printf("query_env:%s\n",getenv("QUERY_STRING="));
//				printf("query_string:%s\n",query_env);
//				printf("get query_env\n");
			}
			else
			{
				char content_env[512];
				memset(content_env,'\0',sizeof(content_env));
        			sprintf(content_env,"Content_Length=%d",content_length);
        			putenv(content_env);
//				printf("content_length:%d\n",content_length);
//				printf("content_env:%s\n",content_env);
			}
        		execl(path,path,NULL);
        		exit(0);
    		}
    		else //parent
    		{
			printf("this is parent\n");
        		close(cgi_output[1]);//取消绑定
        		close(cgi_input[0]);
			if(strcasecmp("POST",method)==0)
			{
				for(;i<content_length;++i)
				{
					recv(conn_fd,&c,1,0);
					write(cgi_input[1],&c,1);
				}
			}
        		while( read(cgi_output[0],&c,1) > 0)
            			send(conn_fd,&c,1,0);
        		close(cgi_output[0]);
        		close(cgi_input[1]);
			waitpid(pid,NULL,0);
    		}
    		//waitpid(pid,&status,0);
    		return 0;
	}
	int web_request(int conn_fd)
	{
		char buf[1024];
		int size=1024;
		int i=1;
		int j=0;
		char url[1024];
		char method[512];
		char path[512];
		char *query_string=NULL;
		struct stat st;
		int content_length=0;
		int cgi=0;
		pid_t pid=0;
		pthread_detach(pthread_self());
		memset(path,'\0',sizeof(path));
		memset(url,'\0',sizeof(url));
		memset(buf,'\0',sizeof(buf));//buf 格式为get /url /http/1.1
		size=get_line(conn_fd,buf,sizeof(buf));
		if(size<0)
			printf("web_request get_line error\n");
		cout<<buf;
		i=0;
		while(!isspace(buf[i]) && i<sizeof(buf) && j<sizeof(method)-1)
			method[j++]=buf[i++];
		method[j]='\0';
		//j ,i指向空格
		while(isspace(buf[i])&& i<sizeof(buf))
			i++;
		if(strcasecmp(method,"get") && strcasecmp(method,"post"))
		{
			page_501(conn_fd);
			return 1;
		}
		j=0;
		while(!isspace(buf[i]) && i<sizeof(buf) && j<sizeof(url)-1)
		{	
			url[j++]=buf[i++];//得到url
		}
		if(strcasecmp(method,"GET")==0)
		{
			query_string=url;
			while(*query_string!='?' &&*query_string!='\0')
			{
				query_string++;
			}
			if(*query_string=='?')
			{
				*query_string='\0';
				query_string++;
				cgi=1;
			}
			cout<<"query_string:"<<query_string<<endl;
			cout<<"method :"<<method<<endl;
		}
		if(strcasecmp(method,"POST")==0)
		{
			cout<<"method :"<<method<<endl;
			cgi=1;
		}
		memset(buf,'\0',sizeof(buf));
		cout<<"请求地址 url："<<url<<endl;
		sprintf(path,"www%s",url);//让其到www目录下查找资源
		cout<<"path: "<<path<<endl;
		cout<<"path length:"<<strlen(path)<<endl;	
		if(path[strlen(path)-1]=='/')
			strcat(path,"index.html");//到这里 path中是www/index.html。
		//下面就是判断请求资源的三种情况 （1.目录，2.可执行文件，3.不存在）
		if(stat(path,&st)==-1)//根据文件名，获取文件信息 这里获取文件信息失败
		{
			cout<<"stat error: "<<strerror(errno)<<endl;//因为在www下面没有这个文件 从而error
			size=1;
			while((size>0) && strcmp(buf,"\n")!=0)// clear内核缓冲区中的请求
			{
				size=get_line(conn_fd,buf,sizeof(buf));
			}
			page_404(conn_fd);	
		}
		else//找到这个文件
		{
			if((st.st_mode& S_IFMT)==S_IFDIR)//如果这个文件是目录，就访问该目录下的index.html
			{
				strcat(path,"/index.html");
				cout<<"path: "<<path<<endl;
			}
			if((st.st_mode& S_IXUSR) || (st.st_mode& S_IXGRP)||(st.st_mode& S_IXOTH))//判断url地址对应文件是否可执行，权限
					cgi=1;
			if(cgi==0)//url对应的不是cgi程序，返回静态网页
				web_httppage(conn_fd,path);
			else
				exe_cgi(conn_fd,query_string,path,method);
		}
		close(conn_fd);
		return 0;
	}
	int web_httppage(int conn_fd,char *path)//返回静态网页
	{
		FILE* res=NULL;
		int size=1;
		char type[32]="text/html";
		char *p=type;
		char buf[1024];
		int filesize=0;
		memset(buf,'\0',sizeof(buf));
		while((size>0)&& strcmp(buf,"\n")!=0)//去掉多余的请求头信息
			size=get_line(conn_fd,buf,sizeof(buf));
		size_t len=strlen(path);
    		cout<<path<<":"<<len<<endl;
     		//if(path[len-4]=='h'&&path[len-3]=='t'&&path[len-2]=='m'&&path[len-1]=='l')
   		{
         		//strcpy(type,"text/html");
     		}
     		//else if(path[len-4]=='.'&&path[len-3]=='i'&&path[len-2]=='c'&&path[len-1]=='o')
     		{
         		//strcpy(type,"image/x-icon");
     		}
     		//else if(path[len-4]=='.'&&path[len-3]=='j'&&path[len-2]=='p'&&path[len-1]=='g')
     		{
         		//strcpy(type,"image/jpg");
     		}
     		//else
     		{
         		//strcpy(type,"text/html");
     		}
		res=fopen(path,"r");
		if(res==NULL)
			page_404(conn_fd);
		else
		{
			page_Headrs(conn_fd,p);
			page_cat(conn_fd,res);
		}
		fclose(res);
		return 0;
	}
	int get_filetype(char *path,char *type)
	{
		if(strstr(path,".html"))
 		        strcpy(type,"text/html");
	        else if(strstr(path,".gif"))
          		strcpy(type,"image/gif");
     		else if(strstr(path,".jpg"))
          		strcpy(type,"image/jpeg");
      		else if(strstr(path,".png"))
         		strcpy(type,"image/png");
     		else if(strstr(path,".ico"))
         		strcpy(type,"image/x-icon");
     	        else if(strstr(path,".js"))
         	 	strcpy(type,"text/javascript");
     		else if(strstr(path,".json"))
	        	 strcpy(type,"application/json");
    	 	else if(strstr(path,".css"))
         		strcpy(type,"text/css"); 
	   	else
    			strcpy(type,"text/plain");    
		return 0;

	}
	int page_Headrs(int conn_fd,char* type)
	{
		char buf[1024];
		strcpy(buf,"HTTP/1.1 200 OK\r\n");
		send(conn_fd,buf,strlen(buf),0);
		sprintf(buf,"server zyw117\r\n");
		send(conn_fd,buf,strlen(buf),0);
		sprintf(buf,"Content-Type: %s\r\n",type);//这添加type 类型
		send(conn_fd,buf,strlen(buf),0);
		sprintf(buf,"\r\n");
		send(conn_fd,buf,strlen(buf),0);
		return 0;
	}
	int page_cat(int conn_fd,FILE* fp)
	{
		char buf[1024];
		int c=fgetc(fp);
		while(c!=EOF)
		{
			send(conn_fd,&c,1,0);
			c=fgetc(fp);
		}
		return 0;
	}
	int get_line(int clifd,char *buf,int size)//行读取
	{
		int i=0;
        	char c='\0';
        	int n=0;
        	while(i< size &&c!='\n')
       	 	{
                	n=recv(clifd,&c,1,0);
                	if(n>0)
                	{
                        	if(c=='\r')
                        	{
                                	n=recv(clifd,&c,1,MSG_PEEK);
                                	if(n>0 && c=='\n')
                                	{
                                        	n=recv(clifd,&c,1,0);
                                	}
                                	else
                                        	c='\n';
                        	}
                        	buf[i]=c;
                        	i++;
                	}
                	else
                        	c='\n';
        	}
        	buf[i]='\0';
        	return i;	
	}
	int page_404(int cli_fd)
	{
		char buf[1024];
		sprintf(buf,"404\r\n");
		send(cli_fd,buf,strlen(buf),0);
		return 0;
	}
	int page_500(int cli_fd)
	{
		char buf[1024];
		sprintf(buf,"500 \r\n");
		send(cli_fd,buf,strlen(buf),0);
		return 0;
	}
	int page_200(int cli_fd)
	{
		char buf[1024];
		sprintf(buf, "HTTP/1.1 200 OK\r\n");
		send(cli_fd, buf, strlen(buf), 0);
		sprintf(buf, "Server:zywhttpweb.com\r\n");
		send(cli_fd, buf, strlen(buf), 0);
	    	sprintf(buf, "Content-Type: text/html\r\n");
		send(cli_fd, buf, strlen(buf), 0);
        	sprintf(buf, "\r\n");
    		send(cli_fd, buf, strlen(buf), 0);    
		//sprintf(buf, "<HTML><HEAD><TITLE>Hello World\r\n");
   		//send(cli_fd, buf, strlen(buf), 0);
   		//sprintf(buf, "</TITLE></HEAD>\r\n");
	    	//send(cli_fd, buf, strlen(buf), 0);
	     	//sprintf(buf, "<BODY><h1>Hello World</h1>\r\n");
        	//send(cli_fd, buf, strlen(buf), 0);
     		//sprintf(buf, "</BODY></HTML>\r\n");
	     	//send(cli_fd, buf, strlen(buf), 0);
		return 0;
	}
	int page_501(int cli_fd)
	{
		char buf[1024];
		sprintf(buf,"HTTP/1.1 501 Method Not Implemented\r\n");
		send(cli_fd,buf,strlen(buf),0);
		sprintf(buf, "Server:zywhttpweb.com\r\n");
		send(cli_fd, buf, strlen(buf), 0);
	    	sprintf(buf, "Content-Type: text/html\r\n");
		send(cli_fd, buf, strlen(buf), 0);
		sprintf(buf,"\r\n");
		send(cli_fd,buf,strlen(buf),0);
		sprintf(buf,"<HTML><HEAD><TITLE>Method Not Implemented\r\n");
		send(cli_fd,buf,strlen(buf),0);
		sprintf(buf,"</TITLE></HEAD>\r\n");
		send(cli_fd,buf,strlen(buf),0);
		sprintf(buf,"<BODY><P>HTTP request method not supported");
		send(cli_fd,buf,strlen(buf),0);
		sprintf(buf,"</BODY></HTML>\r\n");
		send(cli_fd,buf,strlen(buf),0);		
		return 0;
	}


	int web_close()
	{
		close(httpd);
		return 0;
	}
private:
	int httpd;
};
