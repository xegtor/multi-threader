#include <chrono>
#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>
#include <pthread.h>
#include <vector>
#include <mutex>

std::mutex mtx;
int chunk_size = 0;
int chunk_size2 = 0;
int chunk_size1 = 0;

void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads);
void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads);

int user_main(int argc, char **argv);

/* Demonstration on how to pass lambda as parameter.
 * "&&" means r-value reference. You may read about it online.
 */
void demonstration(std::function<void()> && lambda) {
  lambda();
}

int main(int argc, char **argv) {
  /* 
   * Declaration of a sample C++ lambda function
   * that captures variable 'x' by value and 'y'
   * by reference. Global variables are by default
   * captured by reference and are not to be supplied
   * in the capture list. Only local variables must be 
   * explicity captured if they are used inside lambda.
   */
  int x=5,y=1;
  // Declaring a lambda expression that accepts void type parameter
  auto /*name*/ lambda1 = /*capture list*/[/*by value*/ x, /*by reference*/ &y](void) {
    /* Any changes to 'x' will throw compilation error as x is captured by value */
    y = 5;
    std::cout<<"====== Welcome to Assignment-"<<y<<" of the CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  // Executing the lambda function
  demonstration(lambda1); // the value of x is still 5, but the value of y is now 5

  int rc = user_main(argc, argv);
 
  auto /*name*/ lambda2 = [/*nothing captured*/]() {
    std::cout<<"====== Hope you enjoyed CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  demonstration(lambda2);
  return rc;
}

#define main user_main

void* parallel_for_helper(void* arg) {
    auto data = static_cast<std::pair<std::function<void(int)>, int>*>(arg);
    for (int i = data->second; i < data->second + chunk_size; ++i) {
        data->first(i);
    }
//    std::lock_guard<std::mutex> lock(mtx);
//    delete data;
    return nullptr;
}

void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads) {
    auto start_time = std::chrono::high_resolution_clock::now();  // Record start time

    int range = high - low + 1;
    chunk_size = range / numThreads;
    std::vector<pthread_t> threads(numThreads);

    std::vector<std::pair<std::function<void(int)>, int>*> dataVector(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        int thread_low = low + i * chunk_size;

        dataVector[i] = new std::pair<std::function<void(int)>, int>(lambda, thread_low);

        if (pthread_create(&threads[i], nullptr, parallel_for_helper, dataVector[i]) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            return;
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
        delete dataVector[i];  // Delete data after thread completion
    }

    auto end_time = std::chrono::high_resolution_clock::now();  // Record end time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Total execution time: " << duration.count() << " microseconds" << std::endl;
}

void* parallel_for_2d_helper(void* arg) {
    auto data = static_cast<std::pair<std::function<void(int, int)>, std::pair<int, int> >*>(arg);
    data->first(data->second.first, data->second.second);
    delete data;
    return nullptr;
}

void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads) {
    auto start_time = std::chrono::high_resolution_clock::now();  // Record start time

    int range1 = high1 - low1 + 1;
    int range2 = high2 - low2 + 1;
    chunk_size1 = range1 / numThreads;
    chunk_size2 = range2 / numThreads;
    std::vector<pthread_t> threads(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        int thread_low1 = low1 + i * chunk_size1;
        int thread_low2 = low2 + i * chunk_size2;

        auto data = new std::pair<std::function<void(int, int)>, std::pair<int, int>>(
                lambda, std::make_pair(thread_low1, thread_low2));

        if (pthread_create(&threads[i], nullptr, parallel_for_2d_helper, data) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            return;
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    auto end_time = std::chrono::high_resolution_clock::now();  // Record end time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Total execution time: " << duration.count() << " microseconds" << std::endl;
}