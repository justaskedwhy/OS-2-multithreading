#ifndef __ASSIGN3__
    #define __ASSIGN3__
    #include <pthread.h>
    #include <semaphore.h>
    #include <atomic>
    #include <random>
    
    // enum for the type of lock to be used in the producer and consumer process.
    typedef enum {sem,lock} lock_type; 
    
    // struct for the arguments and required variables for the producers and consumers implementation.
    struct job_args {
        // variables for tasks.
        int no_of_plates; // cnt_p/c
        void * task_args; // process aruguments
        lock_type type;
        int * task_no_SG;
        
        // variables for locks. (should be intialized before use!!!)
        pthread_mutex_t * lock; // lock for placing the plates.
        void * full; // semaphore/condition for full plates.
        void * empty; // semaphores/condition for empty plates.

        // atomic variable for full plates (implemented for binary semaphores and lock).
        std::atomic<int> * full_plates; 
        // atomic variable for empty plates (implemented for binary semaphores and lock).
        std::atomic<int> * empty_plates;

        // delay related variables.
        double avg_delay; // mu_p/c

        //log file.
        FILE * log;
    };
    void producer(bool (*process)(void * args), job_args args); // function for the producer.
    void consumer(bool (*process)(void * args), job_args args); // function for the consumer.
    
    #include "Assign3.cpp"
#endif