#include "Src/Assign3.hpp"
#include "Src/tasks.cpp"

char default_log[] = "main.log";

void * wrapper_p (void * args);
void * wrapper_c (void * args);

int main(int argc,char * argv[]){
    using namespace std;
    string input_file;
    string output_file;
    char * log_file;
    if (argc <= 2 || argc > 4) {
        throw "Usage: <executable_file> <input file path> <output file path> <log file path (def = main.log)>";
        return 1;
    }
    else if (argc == 3){
        input_file = argv[1];
        output_file = argv[2];
        log_file = default_log;
    }
    else{
        input_file = argv[1];
        output_file = argv[2];
        log_file = argv[3];
    }

    //input file--
    ifstream file(input_file);

    int capacity,n_p,n_c,cnt_p,cnt_c,mu_p,mu_c;
    file >> capacity >> n_p >> n_c >> cnt_p >> cnt_c >> mu_p >> mu_c;

    file.close();
    //--

    //output file--
    ofstream out(output_file);
    //--

    //log file--
    FILE * log = fopen(log_file,"w");
    //--

    // plate rack
    queue<string> buffer;

    // locks initalization and declaration --
    pthread_mutex_t lock;
    pthread_cond_t full, empty;
    {
    pthread_mutex_init(&lock, nullptr);
    pthread_cond_init(&full, nullptr);
    pthread_cond_init(&empty, nullptr);
    }
    //--   

    // atomic global variables delcaraion--
    atomic<int> full_plates(0);
    atomic<int> empty_plates(capacity);
    //--

    // producer/consumer job parameters initalization--
    job_args producer_args,consumer_args;
    {
    producer_args.no_of_plates = cnt_p;
    producer_args.type = lock_type::lock;
    producer_args.lock = &lock;
    producer_args.full = &full;
    producer_args.empty = &empty;
    producer_args.full_plates = &full_plates;
    producer_args.empty_plates = &empty_plates;
    producer_args.avg_delay = mu_p;
    producer_args.log = log;

    consumer_args.no_of_plates = cnt_c;
    consumer_args.type = lock_type::lock;
    consumer_args.lock = &lock;
    consumer_args.full = &full;
    consumer_args.empty = &empty;
    consumer_args.full_plates = &full_plates;
    consumer_args.empty_plates = &empty_plates;
    consumer_args.avg_delay = mu_c;
    consumer_args.log = log;
    }
    //--
    
    // producer/consumer task arguments initialization--
    function_args<string> producer_task_args,consumer_task_args;
    {
        producer_task_args.item_no = 0;
        producer_task_args.buffer = &buffer;
        producer_task_args.out = &out;
        consumer_task_args.item_no = 0;
        consumer_task_args.buffer = &buffer;
        consumer_task_args.out = &out;
    }
    //--
    
    // task fixation in job args--
    producer_args.task_args = &producer_task_args;
    consumer_args.task_args = &consumer_task_args;
    producer_args.task_no_SG = &producer_task_args.item_no;
    consumer_args.task_no_SG = &consumer_task_args.item_no;
    //--
    
    // thread creation--
    pthread_t producer_t[n_p],consumer_t[n_c];
    for (int i = 0;i < n_p;i++){
        pthread_create(&producer_t[i],nullptr,wrapper_p,&producer_args);
    }
    for (int i = 0;i < n_c;i++){
        pthread_create(&consumer_t[i],nullptr,wrapper_c,&consumer_args);
    }
    //--
    
    // a long wait --
    for (int i = 0;i < n_p;i++){
        pthread_join(producer_t[i],nullptr);
    }
    for (int i = 0;i < n_c;i++){
        pthread_join(consumer_t[i],nullptr);
    }
    //--
    
    // file close--
    out.close();
    //--

    // freeing dynamic memory--
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
    //--

    return 0;
}

void * wrapper_p (void * args){
    job_args * arg = (job_args *) args;
    producer(task_p<std::string>,*arg);
    return nullptr;
}
void * wrapper_c (void * args){
    job_args * arg = (job_args *) args;
    consumer(task_c<std::string>,*arg);
    return nullptr;
}