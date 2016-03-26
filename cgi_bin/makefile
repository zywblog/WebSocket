
CC=gcc
CX=g++
INCLUDE=-I ../ -I ../mysql_connect/mysqlconn_lib/mysql_lib/include -I ../mysql_connect/ 
LIB=-L ../mysql_connect/mysqlconn_lib/mysql_lib/lib

.PHONY:all
all: regstic cgi_bin_exe
regstic:regstic.cpp ../mysql_connect/sql_connect.cpp
	$(CX) -o $@ $^ $(INCLUDE) $(LIB) -lmysqlclient
cgi_bin_exe:cgi_bin.c
	$(CC) $^ -o $@ 
.PHONY:ouput
output:
	cp cgi_bin_exe ../www/cgi_bin/
	cp $(shell pwd)/cgi.html ../www/cgi_bin/
	cp $(shell pwd)/regstic.html ../www/cgi_bin
	cp $(shell pwd)/regstic ../www/cgi_bin
.PHONY:clean
clean:
	rm -f cgi_bin_exe
	rm -f regstic
	rm -f ../www/cgi_bin/cgi_bin_exe
	rm -f ../www/cgi_bin/cgi.html
	rm -f ../www/cgi_bin/regstic.html
	rm -f ../www/cgi_bin/regstic

