WORK_PATH=$(shell pwd)
BIN=sql_connect
BIN_SRC=sql_connect.h sql_connect.cpp
CC=g++
INCLUDE=-I ${WORK_PATH}/mysqlconn_lib/mysql_lib/include -I.
LIB=-L ${WORK_PATH}/mysqlconn_lib/mysql_lib/lib
LIB_NAME=-lmysqlclient 
FLAGS=-g -o

$(BIN):$(BIN_SRC)
	$(CC) $(FLAGS) $@ $^ $(LIB) $(INCLUDE) $(LIB_NAME)
.PHONY:clean
clean:
	rm -f $(BIN)
