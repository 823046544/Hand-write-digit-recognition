GFLAG = -I. -lm -lX11 -lpthread \
		-Wfatal-errors -std=c++11 -pedantic\
		-Dcimg_use_vt100 -g -w \
		`pkg-config opencv --libs --cflags` \
	
objects = main.cpp

TARGET = main

main: $(objects)
	g++ -o main $(objects) $(GFLAG)

