name = gobang
objs = gobang.o
objects = $(objs)
opt = -Wall -std=c99 -O2

all: $(name)

$(name): $(objects)
	cc $(opt) -o $(name) $(objects)

gobang.o: gobang.c
	cc $(opt) -c gobang.c -o gobang.o

clean:
	rm -f $(objects) $(name)