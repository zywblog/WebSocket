PWD=$(shell pwd)
SER_BIN=webserver
SER_SRC=webserver.cpp
CC=gcc
FLAGS=-g -o

#webserver:webserver.cpp
	#gcc $^ -std=c++0x -g -o $@
#	gcc -g -o $@ $^ -lstdc++
WORK_PATH=$(shell pwd)
.PHONY:all
#all:$(SER_BIN) cgi
$(SER_BIN):$(SER_SRC)
	$(CC) $(FLAGS) $@ $^ -lstdc++ -lpthread
.PHONY:cgi

.PHONY:output
ouput:
		
.PHONY:clean
clean:
	rm -f webserver
