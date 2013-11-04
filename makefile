treesort: treesort.c
	gcc -x c -o treesort treesort.c -W -Wall

clean:
	rm -f *.o
