##
# Poor man's Chroma
#
# @file
# @version 0.1
all: compile run

compile:
	gcc main.c -o poormansChroma -lX11
run:
	./poormansChroma


# end
