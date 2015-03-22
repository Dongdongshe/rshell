#!/bin/bash
#run server using tcpserver
nohup tcpserver localhost 8080 ./http-stdin.sh &
