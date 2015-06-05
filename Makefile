main: main.c
	gcc -o main -Iinclude/ `sdl2-config --prefix=. --cflags --libs` -framework OpenGL main.c
