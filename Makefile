main:
	gcc $(CFLAGS) populate.c -o populate
	gcc $(CFLAGS) part1dfs.c -o part1dfs
	gcc $(CFLAGS) part1bfs.c -o part1bfs
	gcc $(CFLAGS) part2.c -o part2
remove:
	rm -f *.o populate part1dfs part1bfs part2
