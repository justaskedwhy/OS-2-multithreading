#include "Src_header.h"
#include <vector>
#include <cmath>
#include <iostream>

// local lock functions to implement locks for synchronization --
void lock(std::atomic_flag * tas_lock);
void lock(cas_lock * cas_lock,bool bound__ = false);
void unlock(std::atomic_flag * tas_lock);
void unlock(cas_lock * cas_lock,bool bound__ = false);
//--

//struct to be passed in pthread_cleanup_push (did not implement) --
typedef struct should_free{
    std::vector<long double> * ret;
} should_free;
//--

//function to validate a part of sudoku returns a boolean wether the validation is successful or not --
bool check_part(check_params params){
    std::vector<bool> check_vector(params.block_height*params.block_length + 1,false);
    bool k = true;
    switch (params.search_type){
        case 'r':
            for (int i = 0;i < params.block_length*params.block_height && k;i++){
                if (!check_vector[params.sudoku[params.label][i]]){
                    check_vector[params.sudoku[params.label][i]] = true;
                }
                else{
                    k = false;
                }
            }
            break;
        case 'c':
            for (int i = 0;i < params.block_length*params.block_height && k;i++){
                if (!check_vector[params.sudoku[i][params.label]]){
                    check_vector[params.sudoku[i][params.label]] = true;
                }
                else{
                    k = false;
                }
            }
            break;
        case 'b':
            for (int i = 0;i < params.block_length*params.block_height && k;i++){
                if (!check_vector[params.sudoku[(params.label/params.block_length)*params.block_height + i/params.block_length][(params.label%params.block_length)*params.block_length + i%params.block_length]]){
                    check_vector[params.sudoku[(params.label/params.block_length)*params.block_height + i/params.block_length][(params.label%params.block_length)*params.block_length + i%params.block_length]] = true;
                }
                else{
                    k = false;
                }
            }
            break;
        default:
            throw "Invalid search type";
            break;
    }
    return k;
}
//--

// main thread function-- refer the report for more detail
thread_params fetch_params(std::ifstream &file){
    thread_params tparams;
    int block_square;
    file >> tparams.no_of_threads;
    file >> block_square;
    file >> tparams.task_increament;
    tparams.cparmas.sudoku = new int*[block_square];
    for (int i = 0;i < block_square;i++){
        tparams.cparmas.sudoku[i] = new int[block_square];
    }
    for (int i = 0;i < block_square;i++){
        for (int j = 0;j < block_square;j++){
            file >> tparams.cparmas.sudoku[i][j];
        }
    }
    tparams.cparmas.block_height = (int) sqrt(block_square);
    tparams.cparmas.block_length = tparams.cparmas.block_height;
    tparams.cparmas.label = 0;
    tparams.cparmas.search_type = 'r';
    tparams.lock_type = "tas";
    return tparams;
}
void * thread_main(void * params){
    should_free f_;
    thread_params * param = (thread_params *) params;
    thread_params tparams = *param;
    pthread_t self_id = pthread_self();
    size_t sudoku_size = (tparams.cparmas.block_height*tparams.cparmas.block_length);
    long double CSout1 = 0,CSin = 0,CSout2 = 0,count = 0,sumen = 0,sumex = 0;
    int local_task_count;
    
    f_.ret = new std::vector<long double>();
    while (tparams.task_count->load() < sudoku_size*3 && *tparams.valid){
        
        {fprintf(tparams.file.out,"Thread %ld requests to enter CS1 at %Lfs.\n",self_id,CSout1 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - tparams.anchor_time).count()/1e9);}
        Lock(tparams);
        //-- Critical Section
        {fprintf(tparams.file.out,"Thread %ld enters CS1 at %Lfs.\n",self_id,CSin = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - tparams.anchor_time).count()/1e9);}
        local_task_count = tparams.task_count->load();
        if (*tparams.task_count >= sudoku_size*3 && *tparams.valid){
            unlock(tparams.lock);
            break;
        }
        tparams.task_count->fetch_add(tparams.task_increament);
        //-- 
        Unlock(tparams);
        {fprintf(tparams.file.out,"Thread %ld leaves CS1 at %Lfs.\n",self_id,CSout2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - tparams.anchor_time).count()/1e9);}
        for (int i = 0;i < tparams.task_increament && (local_task_count + i < 3*sudoku_size) && *tparams.valid ;i++){
            
            tparams.cparmas.search_type = ((local_task_count + i)/sudoku_size) ? (((local_task_count + i)/sudoku_size == 1) ? 'c' : 'b') : 'r';
            tparams.cparmas.label = (local_task_count + i)%sudoku_size;
            fprintf(tparams.file.out,"Thread %ld grabs %s %ld at %fs.\n",self_id,(tparams.cparmas.search_type == 'r') ? "row" : (tparams.cparmas.search_type == 'c' ? "column" : "sub-grid"),tparams.cparmas.label,std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - tparams.anchor_time).count()/1e9);
            if (check_part(tparams.cparmas)){
                fprintf(tparams.file.out,"Thread %ld completes checking of %s %ld at %fs and finds it as valid.\n",self_id,(tparams.cparmas.search_type == 'r') ? "row" : (tparams.cparmas.search_type == 'c' ? "column" : "sub-grid"),tparams.cparmas.label,std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - tparams.anchor_time).count()/1e9);
            }
            else{
                fprintf(tparams.file.out,"Thread %ld completes checking of %s %ld at %fs and finds it as invalid.\n",self_id,(tparams.cparmas.search_type == 'r') ? "row" : (tparams.cparmas.search_type == 'c' ? "column" : "sub-grid"),tparams.cparmas.label,std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - tparams.anchor_time).count()/1e9);
                *tparams.valid = false;
                break;
            }
        }
        sumen += std::max(CSin - CSout1,0.0L);
        sumex += std::max(CSout2 - CSin,0.0L);
        count++;
    }
    f_.ret->push_back(sumen/(count ? count : 1));
    f_.ret->push_back(sumex/(count ? count : 1));
    pthread_exit(f_.ret);
}
//--

// definition for lock and unlock--
void lock(std::atomic_flag * tas_lock){
    while (tas_lock->test_and_set());
}
void unlock(std::atomic_flag * tas_lock){
    tas_lock->clear();
}
void lock(cas_lock * cas_lock,bool bound__){
    int cas_expect,self_ticket;
    if (bound__) self_ticket = cas_lock->total_tickets.fetch_add(1);
    do{
        cas_expect = 0;
    }while ((!bound__ && !cas_lock->cas_val.compare_exchange_weak(cas_expect,0,std::memory_order_acquire)) || (!cas_lock->cas_val.compare_exchange_weak(cas_expect,0,std::memory_order_acquire) && self_ticket == cas_lock->curr_serve.load()));
}
void unlock(cas_lock * cas_lock,bool bound__){
    if (bound__) cas_lock->curr_serve.fetch_add(1); 
    cas_lock->cas_val.store(0, std::memory_order_release);
}
//--

// functions which encapsulate call lock and unlock functions --
void Lock(thread_params t__){
    if (t__.lock_type == "tas") lock(t__.lock);
    else if (t__.lock_type == "cas") lock(t__.cas);
    else if (t__.lock_type == "casb") lock(t__.cas,true);
    else{
        fprintf(t__.file.log,"Error at line %d in file %s\n",148,"Src_header.cpp");
        throw "Unkown lock method mentioned";
    }
}
//
void Unlock(thread_params t__){
    if (t__.lock_type == "tas") unlock(t__.lock);
    else if (t__.lock_type == "cas") unlock(t__.cas);
    else if (t__.lock_type == "casb") unlock(t__.cas,true);
    else{
        fprintf(t__.file.log,"Error at line %d in file %s\n",157,"Src_header.cpp");
        throw "Unkown lock method mentioned";  
    } 
}
//--

//method to close all file pointers in file__ struct --
bool file__::close() const{
    bool k = 1;
    k &= fclose(out);
    k &= fclose(log);
    return k;
}
//--