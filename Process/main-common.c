#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

// Assuming these are defined elsewhere in your project
#define MAX_THREADS 100
#define EMPTY -1
#define END_OF_STREAM -1

// Global variables (These must be declared if not in a header)
int loops, max, consumers, producers;
int do_trace = 0, do_timing = 0;
int *buffer;
int num_full = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill_cv = PTHREAD_COND_INITIALIZER;

// Mock-up placeholders for your specific lab functions
void parse_pause_string(char* s, char* t, int n, int* times) {}
void do_fill(int value) { /* your logic to add to buffer */ }
void do_eos() { /* your logic for end of stream */ }
void do_print_headers() {}
void *producer(void *arg) { return NULL; }
void *consumer(void *arg) { return NULL; }
double Time_GetSeconds() { return 0.0; } 
// OSTEP wrapper replacements if not using the common header
#define Pthread_create pthread_create
#define Pthread_join pthread_join
#define Mutex_lock pthread_mutex_lock
#define Mutex_unlock pthread_mutex_unlock
#define Cond_wait pthread_cond_wait
#define Cond_signal pthread_cond_signal
#define Malloc malloc

void usage() {
    fprintf(stderr, "usage: \n");
    fprintf(stderr, "  -l <number of items each producer produces>\n");
    fprintf(stderr, "  -m <size of the shared producer/consumer buffer>\n");
    fprintf(stderr, "  -p <number of producers>\n");
    fprintf(stderr, "  -c <number of consumers>\n");
    fprintf(stderr, "  -P <sleep string>\n");
    fprintf(stderr, "  -C <sleep string>\n");
    fprintf(stderr, "  -v [ verbose flag ]\n");
    fprintf(stderr, "  -t [ timing flag ]\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    loops = 1;
    extern char *optarg;
    extern int optind, opterr, optopt;
    max = 1;
    consumers = 1;
    producers = 1;

    char *producer_pause_string = NULL;
    char *consumer_pause_string = NULL;

    int c;
    while ((c = getopt(argc, argv, "l:m:p:c:P:C:vt")) != -1) {
        switch (c) {
            case 'l': loops = atoi(optarg); break;
            case 'm': max = atoi(optarg); break;
            case 'p': producers = atoi(optarg); break;
            case 'c': consumers = atoi(optarg); break;
            case 'P': producer_pause_string = optarg; break;
            case 'C': consumer_pause_string = optarg; break;
            case 'v': do_trace = 1; break;
            case 't': do_timing = 1; break;
            default:  usage();
        }
    }

    assert(loops > 0);
    assert(max > 0);
    assert(producers <= MAX_THREADS);
    assert(consumers <= MAX_THREADS);

    // Initialization of shared memory
    buffer = (int *) Malloc(max * sizeof(int));
    for (int i = 0; i < max; i++) {
        buffer[i] = EMPTY;
    }

    do_print_headers();
    double t1 = Time_GetSeconds();

    pthread_t pid[MAX_THREADS], cid[MAX_THREADS];
    
    // Creating Producer Threads
    for (long long i = 0; i < producers; i++) {
        Pthread_create(&pid[i], NULL, producer, (void *) i); 
    }
    // Creating Consumer Threads
    for (long long i = 0; i < consumers; i++) {
        Pthread_create(&cid[i], NULL, consumer, (void *) i); 
    }

    // Wait for all Producers
    for (int i = 0; i < producers; i++) {
        Pthread_join(pid[i], NULL); 
    }

    // Properly signal Consumers to exit
    for (int i = 0; i < consumers; i++) {
        Mutex_lock(&m);
        while (num_full == max) 
            Cond_wait(&empty_cv, &m);
        do_fill(END_OF_STREAM);
        do_eos();
        Cond_signal(&fill_cv);
        Mutex_unlock(&m);
    }

    // Collect consumer counts via join
    void *counts[MAX_THREADS]; // Using void* array for thread returns
    for (int i = 0; i < consumers; i++) {
        Pthread_join(cid[i], &counts[i]); 
    }

    double t2 = Time_GetSeconds();

    if (do_trace) {
        printf("\nConsumer consumption:\n");
        for (int i = 0; i < consumers; i++) {
            printf("  C%d -> %ld\n", i, (long)counts[i]);
        }
    }

    if (do_timing) {
        printf("Total time: %.2f seconds\n", t2-t1);
    }

    return 0;
}