TPgraph: TPgraph.o libisentlib.a
	gcc -Wall TPgraph.o -o TPgraph libisentlib.a -lm -lglut -lGL -lX11
TPgraph.o: TPgraph.c GfxLib.h BmpLib.h ESLib.h
	gcc -Wall -c TPgraph.c
