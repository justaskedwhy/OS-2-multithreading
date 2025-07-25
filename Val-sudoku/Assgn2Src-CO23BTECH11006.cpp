#include "./Src/Src_header.h"
#include <iostream>

//global variables--
std::atomic<int> tasks(0); // task count
struct cas_lock cas_var; // lock variable for compare and swap. {both bounded and normal}
std::atomic_flag TAS_flag; // lock variable for test and set.
bool complete = true; // central boolean for early termination. {fallback state : false}
//--

// sequential part--
void sequential(check_params c_);
//--

int main(int argc,char * argv[]){
    using namespace std;
    string lock_type;
    string input_file;\
    char * outfile;
    if (argc <= 3){
        throw "Usage: <executable_file> <input file path> <output file path> <lock_type>{\"tas\",\"cas\",\"casb\",\"seq\"}";
        return 1;
    }
    else{
        input_file = argv[1];
        outfile = argv[2];
        lock_type = argv[3];
    }

    //input file--
    ifstream file(input_file);
    //--

    //temp_objects--
    vector<long double> CS_entry,CS_exit;
    //--

    //central thread parameter struct--
    thread_params init_params = fetch_params(file); // declaration and initialization.
    init_params.anchor_time = chrono::high_resolution_clock::now(); // central anchor time .
    init_params.file.out = fopen(outfile,"w"); // file pointers for output.
    init_params.file.log = fopen("log.txt","w"); // errors.
    init_params.task_count = &tasks; // global task counter.
    init_params.lock = &TAS_flag; // test and set lock object.
    init_params.cas = &cas_var; // compare and swap lock object. {both bounded and normal}
    init_params.lock_type = lock_type; // lock type. {curr available --> tas,cas,casb}
    init_params.valid = &complete; // global variable for early termination (bool).
    //--

    if (init_params.lock_type == "seq"){
        sequential(init_params.cparmas);
    }

    else{
        pthread_t thread_id_array[init_params.no_of_threads]; //array for the storage of thread_t values.
    
        //thread execution part--
        for (int i = 0;i < init_params.no_of_threads;i++){
            pthread_create(thread_id_array + i,nullptr,thread_main,&init_params); // thread creation
        }
        for (int i = 0;i < init_params.no_of_threads;i++){
            vector<long double> * res;
            pthread_join(thread_id_array[i],(void **)(&res));
            if (res != nullptr){
                CS_entry.push_back(res->operator[](0));
                CS_exit.push_back(res->operator[](1));
            }
            delete res;
        }
        //--
    }

    //general stats--
    fprintf(init_params.file.out,"Sudoku is %s.\n\n",(complete ? "Valid" : "Invalid"));
    fprintf(init_params.file.out,"The total time taken is %fs\n",chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - init_params.anchor_time).count()/1e9);
    //--

    if (init_params.lock_type != "seq"){
        long double avgen = 0,avgex = 0,men = 0,mex = 0;
        for (int i = 0;i < CS_entry.size();i++){
            if (men < CS_entry[i]) men = CS_entry[i];
            if (mex < CS_exit[i]) mex = CS_exit[i];
            avgen += CS_entry[i];
            avgex += CS_exit[i];
        }
    
        //thread program stats--
        avgen /= CS_entry.size();
        avgex /= CS_entry.size();
        fprintf(init_params.file.out,"Average time taken by a thread to enter the CS is %Lf microsecond\n",avgen*1e6);
        fprintf(init_params.file.out,"Average time taken by a thread to exit the CS is %Lf microsecond\n",avgex*1e6);
        fprintf(init_params.file.out,"Worst-case time taken by a thread to enter the CS is %Lf microsecond\n",men*1e6);
        fprintf(init_params.file.out,"Worst-case time taken by a thread to exit the CS is %Lf microsecond\n",mex*1e6);
        //--
    }

    //file closure--
    init_params.file.close();
    //--

    //freeing dynamic memory--
    for (int i = 0;i < init_params.cparmas.block_height*init_params.cparmas.block_length;i++) delete [] init_params.cparmas.sudoku[i];
    delete [] init_params.cparmas.sudoku;
    //--

    return 0;
}

//sequential declaration--
void sequential(check_params c_){
    size_t block_size = c_.block_height*c_.block_length;
    char ct[3] = {'r','c','b'};
    for (int t = 0;t < 3;t++){
        c_.search_type = ct[t];
        for (int i = 0;i < block_size;i++){
            c_.label = i;
            complete &= check_part(c_);
        }
    }
}
//--