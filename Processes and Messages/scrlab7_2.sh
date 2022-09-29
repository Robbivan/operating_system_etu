#!/bin/bash

g++	-c	lab7_2.cpp

g++	-o	lab7_2	lab7_2.o		-lpthread -lrt

sudo setcap cap_sys_resource=eip lab7_2
