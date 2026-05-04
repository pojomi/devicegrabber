cc = gcc
bin = devicegrabber
bindir = /usr/bin
src = src/main.c src/dg.c
cflags = -levdev

all: $(src)
	$(cc) $(src) -o $(bin) $(cflags)

install:
	sudo $(cc) $(src) -o $(bindir)/$(bin) $(cflags)

test:
	sudo ./$(bin)

run:
	sudo $(bin)

clean:
	sudo rm $(bin) $(bindir)/$(bin)
