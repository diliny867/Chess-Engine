
# to hide console: -mwindows

chess:
	gcc main.c -o main -O1 -std=c99 -Wall -Wno-parentheses -I ./include/ -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm