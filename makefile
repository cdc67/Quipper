# CS283 Term Project - Quipper
# makefile
#
# Chris Callans
# 3/14

all:
	gcc -o quipper quipper.c omdb.c

quip: 
	@./quipper

bg:
	@./quipper bg &

gdb:
	gcc -orun quipper.c omdb.c -g
	gdb run

finish:
	@pkill quipper


