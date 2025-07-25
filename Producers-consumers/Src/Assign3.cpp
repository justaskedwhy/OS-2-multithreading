#include "Assign3.hpp"
#include <random>
#include <chrono>
#include <unistd.h>
#include <iostream>

typedef pthread_cond_t cond_t;

void producer(bool (*process)(void *args), job_args args)
{
    double total_time = 0;
    std::exponential_distribution<> distribution(1 / args.avg_delay);
    std::random_device random_generator;
    switch (args.type)
    {
    case sem:
        for (int i = 0; i < args.no_of_plates; i++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            sem_wait((sem_t *) args.empty);
            pthread_mutex_lock(args.lock);
            *args.task_no_SG = i + 1;
            if (!process(args.task_args))
                i--;
            pthread_mutex_unlock(args.lock);
            sem_post((sem_t *) args.full);
            usleep((uint8_t) distribution(random_generator)*1000);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            total_time += std::max(elapsed.count(),0.0);
        }
        break;
    case lock_type::lock:
        for (int i = 0;i < args.no_of_plates;i++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            pthread_mutex_lock(args.lock);
            while (args.full_plates->load() == args.empty_plates->load()){
                pthread_cond_wait((cond_t *) args.empty,args.lock);
            }
            *args.task_no_SG = i + 1;
            if (!process(args.task_args))
                i--;
            args.full_plates->fetch_add(1);
            pthread_cond_signal((cond_t *) args.full);
            pthread_mutex_unlock(args.lock);
            usleep((uint8_t) distribution(random_generator)*1000);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            total_time += std::max(elapsed.count(),0.0);
        }
        break;
    default:
        throw "Invalid lock type specified! refer to header or documentation for more information.";
        break;
    }
    fprintf(args.log,"Avg time taken for producer %lu : %f ms\n",pthread_self(),total_time/args.no_of_plates);
}
void consumer(bool (*process)(void *args), job_args args)
{
    double total_time = 0;
    std::exponential_distribution<> distribution(args.avg_delay);
    std::random_device random_generator;
    switch (args.type)
    {
    case sem:
        for (int i = 0; i < args.no_of_plates; i++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            sem_wait((sem_t *) args.full);
            pthread_mutex_lock(args.lock);
            *args.task_no_SG = i + 1;
            if (!process(args.task_args))
                i--;
            pthread_mutex_unlock(args.lock);
            sem_post((sem_t *) args.empty);
            usleep((uint8_t)distribution(random_generator)*1000);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            total_time += std::max(elapsed.count(),0.0);
        }
        break;
    case lock:
        for (int i = 0; i < args.no_of_plates; i++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            pthread_mutex_lock(args.lock);
            while (args.full_plates->load() == 0){
                pthread_cond_wait((cond_t *) args.full,args.lock);
            }
            *args.task_no_SG = i + 1;
            if (!process(args.task_args))
                i--;
            args.full_plates->fetch_sub(1);
            pthread_cond_signal((cond_t *) args.empty);
            pthread_mutex_unlock(args.lock);
            usleep((uint8_t)distribution(random_generator)*1000);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            total_time += std::max(elapsed.count(),0.0);
        }
        break;
    default:
        throw "Invalid lock type specified! refer to header or documentation for more information.";
        break;
    }
    fprintf(args.log,"Avg time taken for consumer %lu : %f ms\n",pthread_self(),total_time/args.no_of_plates);
}