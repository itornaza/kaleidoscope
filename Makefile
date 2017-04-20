SRC := main.c utils.c my_library.c
HEADERS := utils.h my_library.h

CFLAGS := -Wall -Werror -pedantic -O3 -g
LIBS := -ljpeg

all: build

build: $(SRC) $(HEADERS)
	gcc $(CFLAGS) $(SRC) -o imageloader $(LIBS)

valgrind: build
	valgrind --tool=memcheck --show-reachable=yes --leak-check=full -v ./imageloader example.jpg out.jpg

