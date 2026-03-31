cc = gcc
bin = devicegrabber
sh = dg-daemon-script
bindir = /usr/bin
src = main.c dg.c
cflags = -levdev

all: $(src)
	$(cc) $(src) -o $(bin) $(cflags)

install:
	$(cc) $(src) -o $(bindir)/$(bin) $(cflags) && \
	cp $(sh).sh $(bindir)/$(sh)

run:
	./$(bin)

clean:
	rm $(bin) $(bindir)/$(bin) $(bindir)/$(sh)
