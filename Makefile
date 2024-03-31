main:
	gcc $(CFLAGS) populate.c -o populate

remove:
	rm -f *.o populate
