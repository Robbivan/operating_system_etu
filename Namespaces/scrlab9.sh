#!/bin/bash

g++	-c	lab9.cpp

g++	-o	lab9	lab9.o		-lpthread 

sudo setcap cap_sys_admin=eip lab9



