# multi-threader
## Functionality
- This project is for parallelizing lambda functions.
- There are two functions which can be used in any project to parallelize lambda functions, which either take one argument or two arguments.
- The `void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads)` function is for parallelizing one-dimensional array lambda function.
- The `void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads)` function is for parallelizing two-dimensional array lambda function.
- these functions also take in the number of threads to be spawned as arguments so that the user can determine the desired number of threads.
## Implementation
- first, we calculate the chunk size(amount of work each thread does) based on the number of threads and the array size.
- then, we run a loop where we create the thread using `pthread_create` passing in the lambda function, the starting position in the loop, and the helper function.
- there are two helper functions, one for 2d arrays and one for 1d arrays.
- the helper function for the 1D array runs a loop from the start point until the chunk size is over.
- the helper function for the 2D array takes in the lambda function, and the coordinates are converted into a single number, which is then translated into the i and j values until the chunk size is over.
- After the creation process is done, another loop uses `pthread_join` to wait for all the threads to complete.
- after which the threads are deleted.
- In the end, the execution time is printed, which is kept tracked using `std::chrono`.
  ## Contributions
  
  ## GitHub Repository
  - https://github.com/xegtor/multi-threader
