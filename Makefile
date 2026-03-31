src_d = src

all:
	$(MAKE) -C $(src_d) all

install:
	sudo $(MAKE) -C $(src_d) install

run:
	sudo $(MAKE) -C $(src_d) run

clean:
	sudo $(MAKE) -C $(src_d) run
