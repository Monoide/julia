CC = gcc
CCFLAGS = -W -Wall -g -O2
LDLIBS = -lSDL -lm

all: julia

julia: julia.c
	$(CC) $(CCFLAGS) julia.c $(LDLIBS) -o julia
