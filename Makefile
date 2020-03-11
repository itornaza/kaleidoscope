SRC := main.c utils.c my_library.c
HEADERS := utils.h my_library.h

CFLAGS := -Wall -Werror -pedantic -O3 -g
LIBS := -ljpeg

all: build

build: $(SRC) $(HEADERS)
	gcc $(CFLAGS) $(SRC) -o kaleidoscope $(LIBS)

valgrind: build
	valgrind --tool=memcheck --show-reachable=yes --leak-check=full -v ./kaleidoscope example.jpg out.jpg

clean:
	rm -fr out.jpg
	rm -fr kaleidoscope.dSYM
	rm -fr kaleidoscope
