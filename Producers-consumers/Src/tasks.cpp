#include "Assign3.hpp"
#include <queue>
#include <random>
#include <chrono>
#include <sys/time.h>
#include <iostream>
#include <time.h>
#include <iomanip>
#include <fstream>
#include <pthread.h>
template <class T> struct function_args {
    // variables for tasks
    int item_no; // cnt_p/c from the producer,consumer threads.
    std::queue<T> *buffer; // buffer for plates.

    std::ofstream * out;
    //
    
};
template <class T> bool task_p(void * args){
    function_args<T> *arg = (function_args<T> *) args;
    pthread_t self_id = pthread_self();
    T item = T();
    arg->buffer->push(item);
    auto sys_time = std::chrono::high_resolution_clock::now();
    auto sys_time_t = std::chrono::system_clock::to_time_t(sys_time);
    auto sys_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(sys_time.time_since_epoch()) % 1000;
    auto sys_time_mus = std::chrono::duration_cast<std::chrono::microseconds>(sys_time.time_since_epoch()) % 1000;
    std::tm t_info = *std::localtime(&sys_time_t);
    *arg->out << arg->item_no << "th item: " << item << " produced by thread " << self_id << " at " << std::put_time(&t_info, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << sys_time_ms.count() << std::setfill('0') << std::setw(3) << sys_time_mus.count() << " into buffer location " << arg->buffer->size() << std::endl;
    return true;
}

template <class T> bool task_c(void * args){
    function_args<T> *arg = (function_args<T> *) args;
    pthread_t self_id = pthread_self();
    T item = arg->buffer->front();
    arg->buffer->pop();
    auto sys_time = std::chrono::high_resolution_clock::now();
    auto sys_time_t = std::chrono::system_clock::to_time_t(sys_time);
    auto sys_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(sys_time.time_since_epoch()) % 1000;
    auto sys_time_mus = std::chrono::duration_cast<std::chrono::microseconds>(sys_time.time_since_epoch()) % 1000;
    std::tm t_info = *std::localtime(&sys_time_t);
    *arg->out << arg->item_no << "th item: " << item << " consumed by thread " << self_id << " at " << std::put_time(&t_info, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << sys_time_ms.count() << std::setfill('0') << std::setw(3) << sys_time_mus.count() << " into buffer location " << arg->buffer->size() + 1 << std::endl;
    return true;
}