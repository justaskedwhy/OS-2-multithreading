#ifndef __ASSGN2_H__
    #define __ASSGN2_H__
    #include <atomic>
    #include <vector>
    #include <pthread.h>
    #include <chrono>
    #include <fstream>
    //--objects
    struct cas_lock{
        std::atomic<int> cas_val;
        std::atomic<int> curr_serve;
        std::atomic<int> total_tickets;
    };

    typedef struct check_params{
        size_t block_length;
        size_t block_height;
        size_t label;
        int ** sudoku;
        char search_type;
    }check_params;

    struct file__{
        FILE * out;
        FILE * log;
        bool close() const;
    };

    typedef struct thread_params{
        //early termination--
        bool * valid;
        //--
        //lock-- part
        std::string lock_type;
        std::atomic<int> * task_count;
        struct cas_lock * cas;
        std::atomic_flag * lock;
        //--
        //time--
        std::chrono::time_point<std::chrono::high_resolution_clock> anchor_time;
        //--
        //problem paramters--
        size_t no_of_threads;
        size_t task_increament;
        check_params cparmas;
        //--
        //file pointer (input not included)--
        struct file__ file;
        //--
    } thread_params;

    //--

    //lock--
    void Lock(thread_params t__);
    void Unlock(thread_params t__);
    //--

    //thread function--
    void * thread_main(void * params);
    //--
    //-- sodoku check part
    bool check_part(check_params params);
    //--

    //--file i/o part
    thread_params fetch_params(std::ifstream &file);
    //--
#endif