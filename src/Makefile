all: merger merger_test gen test

merger: merger.cpp
	g++ merger.cpp -o merger -lpthread -Wall

merger_test: merger_test.cpp
	g++ merger_test.cpp -o merger_test -lpthread -Wall

gen: gen.cpp
	g++ gen.cpp -o gen -Wall

test: test.cpp
	g++ test.cpp -o test -Wall

clean:
	rm merger merger_test gen test
