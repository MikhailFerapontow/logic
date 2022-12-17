all:
	 g++ -I $(shell pwd)/build/include main.cpp utils.cpp -L $(shell pwd)/build/lib -lbdd -static
ad:
	g++ -I $(shell pwd)/build/include main.cpp utils.cpp -L $(shell pwd)/build/lib -lbdd -static -D ADVANCED