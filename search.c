#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

/*
    TO-DO:
        - Organizar código - separar em arquivos
        - Criar flags (?)
        - Posso colocar o ponteiro de resultado de forma diferente?
*/

typedef struct search_args {
    int *arr;
    int expected;
    int thread_id;
    int start; 
    int end;
    int verbose;
    int *result;
} search_args;

typedef struct generate_args {
    int *arr;
    int start;
    int end;
    int cap;
} generate_args;

int search(int*, int, int, int, int);
void *thread_search(void*);
void generate_arr(int*, int, int, int);
void *thread_generate(void*);

void temp_test();

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 4) {
        temp_test();
        return EXIT_SUCCESS;
    }

    int num_threads = atoi(argv[1]);
    int items = atoi(argv[2]);
    int expected = atoi(argv[3]);

    int *arr = (int*) malloc(items * sizeof(int));;
    if (arr == NULL) {
        printf("Erro na alocação de memória!\n");
        exit(EXIT_FAILURE);
    }

    generate_arr(arr, num_threads, items, 101);

    clock_t start = clock();
    int found = search(arr, num_threads, items, expected, 1);
    printf("Valor encontrado? %s\n", found ? "SIM" : "NAO");
    clock_t end = clock();
    printf("%lf (ms)\n", (double) (end-start)/CLOCKS_PER_SEC);

    free(arr);
    return EXIT_SUCCESS;
}

void temp_test() {
    int num_threads[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
    int items = 1000000;
    int expected = rand() % 101;

    int *arr = (int*) malloc(items * sizeof(int));;
    if (arr == NULL) {
        printf("Erro na alocação de memória!\n");
        exit(EXIT_FAILURE);
    }

    generate_arr(arr, 512, items, 101);

    printf("Fazendo a busca do valor %d em %d items!\n", expected, items);

    for (int i = 0; i < 10; i++) {
        printf("Com %d threads: ", num_threads[i]);
        clock_t start = clock();
        int found = search(arr, num_threads[i], items, expected, 0);
        clock_t end = clock();
        printf("Valor encontrado? %s\n", found ? "SIM" : "NAO");
        printf("%lf (ms)\n", (double) (end-start)/CLOCKS_PER_SEC);
    }

    free(arr);
}

int search(int* arr, int num_threads, int size, int expected, int verbose) {
    pthread_t threads[num_threads];
    int found = 0;

    for (int id = 0; id < num_threads; id++) {
        int items_per_thread = size/num_threads;
        int remaining = size % num_threads;

        int start = (id * items_per_thread) + min(id, remaining);
        int end = start + items_per_thread + (id < remaining ? 1 : 0);

        search_args *args = (search_args*) malloc(sizeof(search_args));
        
        args -> arr = arr;
        args -> expected = expected;
        args -> thread_id = id;
        args -> start = start;
        args -> end = end;
        args -> result = &found;
        args -> verbose = verbose;
        
        int code = pthread_create(&threads[id], NULL, thread_search, (void*) args);
        if (code) {
            printf("Erro ao criar a thread %d\n", id);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return found;
}

void* thread_search(void* args) {
    search_args* arg = (search_args*) args;

    int *arr = arg -> arr;
    int *result = arg -> result;
    int id = arg -> thread_id;
    int start = arg -> start;
    int end = arg -> end;
    int expected = arg -> expected;
    int verbose = arg -> verbose;

    for (int i = start; i < end; i++) {
        if (arr[i] == expected) {
            *(result) = 1;
            if (verbose)
                printf("(Thread #%d) Encontrado o valor %d no index %d!\n", id, expected, i);
        }
    }

    free(args);
    
    pthread_exit(NULL);

    return NULL;
}

void generate_arr(int* arr, int num_threads, int size, int cap) {
    pthread_t threads[num_threads];

    for (int id = 0; id < num_threads; id++) {
        int items_per_thread = size/num_threads;
        int remaining = size % num_threads;

        int start = (id * items_per_thread) + min(id, remaining);
        int end = start + items_per_thread + (id < remaining ? 1 : 0);

        generate_args *args = (generate_args*) malloc(sizeof(generate_args));
        
        args -> arr = arr;
        args -> start = start;
        args -> end = end;
        args -> cap = cap;
        
        int code = pthread_create(&threads[id], NULL, thread_generate, (void*) args);
        if (code) {
            printf("Erro ao criar a thread %d\n", id);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

void *thread_generate(void* args) {
    generate_args* arg = (generate_args*) args;

    int *arr = arg -> arr;
    int start = arg -> start;
    int end = arg -> end;
    int cap = arg -> cap;

    srand(time(NULL) + arg -> start);
    for (int i = start; i < end; i++) {
        arr[i] = rand() % cap;
    }

    free(args);
    
    pthread_exit(NULL);

    return NULL;
}

