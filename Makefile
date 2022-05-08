CC = g++.exe
STRIP = strip.exe

OBJ = obj/main.o obj/parser.o obj/lexer.o obj/eval.o obj/object.o obj/gc.o

all: lispi.exe
#$(STRIP) $(^)

lispi.exe: $(OBJ)
	$(CC) $(^) -o $(@) -g

obj/%.o: %.cpp
	$(CC) -Wall -g -Wno-format -std=c++11 $(^) -o $(@) -c

clean:
	@del obj\*.o
