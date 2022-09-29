#!/bin/bash

g++	-c	lab7_1.cpp

g++	-o	lab7_1	lab7_1.o		-lpthread -lrt

sudo setcap cap_sys_resource=eip lab7_1


