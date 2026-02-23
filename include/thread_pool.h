#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include<iostream>
#include<queue>
#include<vector>
#include<functional>
#include<thread>

class ThreadPool {
    private:
        std::queue<std::function<void()>> tasks;
        std::mutex mutex;
        std::condition_variable cond;
        const unsigned int num_threads;
        // std::vector<pthread_t> threads;
        std::vector<std::thread> threads;
    public:
        ThreadPool(const unsigned int num_threads);
        ~ThreadPool();
        
        // Delete copy constructor and copy assignment
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        
        void start();
        void thread_task();
        void add_task(std::function<void()>);
};
#endif