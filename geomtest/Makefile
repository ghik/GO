all: geomtest

geomtest: *.cpp *.h
	g++ -Wall -Wextra *.cpp `pkg-config --cflags --libs gtk+-2.0` -o $@

.PHONY: clean

clean:
	rm -f geomtest
