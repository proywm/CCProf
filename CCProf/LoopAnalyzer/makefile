all:
	g++ -I./libxml2/include -c treeXtract.cpp -o treeXtract.o
	g++ treeXtract.o -L./libxml2/.libs -lxml2 -lz -lm -o treeXtract
