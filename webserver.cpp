#include"webserver.h"

int main()
{
	webserver ws;
	u_short port=0;
	ws.web_init(port);
	cout<<"init sucess"<<endl;
	int conn_fd=0;
	pid_t pid;
	while(1)
	{
		conn_fd=ws.web_accept();
		ws.web_request(conn_fd);
	}	
	ws.web_close();
	return 0;	
}
