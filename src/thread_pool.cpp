#include "thread_pool.h"

ThreadPool::ThreadPool(const unsigned int num_threads)
    : num_threads(num_threads), threads(num_threads)
{
    threads.reserve(num_threads);
}

ThreadPool::~ThreadPool() {
    // Wait for all threads to finish
    for(auto &thread: threads)
    {
        if(thread.joinable())
        {
            thread.join();
        }
    }
}

void ThreadPool::thread_task()
{
    while(1)
    {
        std::unique_lock<std::mutex> lock(mutex);
        while(tasks.empty())
        {
            cond.wait(lock);
        }
        if(!tasks.empty())
        {
            std::function<void()> task = std::move(tasks.front());
            tasks.pop();
            lock.unlock();
            task();
        }
    }
    return;
}

void ThreadPool::add_task(std::function<void()> f)
{
    {
        std::lock_guard<std::mutex> lock(mutex);
        tasks.push(std::move(f));
    }
    cond.notify_one();
}

void ThreadPool::start()
{
    for(unsigned int i = 0; i < num_threads; i++) {
        threads.emplace_back(&ThreadPool::thread_task, this);
    }
}