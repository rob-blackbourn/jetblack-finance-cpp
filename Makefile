.PHONY: all clean test install uninstall

all:
	cd src && make all
	cd tests && make all

install:
	cd src && make install

uninstall:
	cd src && make uninstall

test:
	cd tests && make test

clean:
	cd src && make clean
	cd tests && make clean
