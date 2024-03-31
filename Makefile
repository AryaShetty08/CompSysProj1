main:
	gcc $(CFLAGS) populate.c -o populate
	gcc $(CFLAGS) part1.c -o part1
	gcc $(CFLAGS) part2.c -o part2
remove:
	rm -f *.o populate part1 part2
