This program generates a series of numbers (positive integers), with a hidden set of
keys (negative numbers -1 to -60) placed randomly throuhgout the file

The program finds these keys for the user using two different tehcniques of forking, with 
DFS and BFS implementation.

Before testing the function, make sure to make all the files executable.
Next you want to run "./populate <number of integers> <number of hidden keys>" first to create the list of numbers
Then you can run "./part1dfs <number of integers> <number of hidden keys> <number of processes>"

The number of integers has to be greater than or equal to 10000, the number of hidden keys has to be greater than or equal to 30 and less than or equal to 60
The number of processes has to be greater than or equal to 1

The arguments in part1dfs should line up with populate
