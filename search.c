#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

/*
    TO-DO:
        - Paralelizar geração aleátoria de números
        - Colocar argumentos na chamada da main
        - Criar sistema de tempo
        - Criar comparativo de com thread vs sem thread
*/

// temp vars
int num_threads = 5;
int items = 200;

void *search(void*);
void generate_arr(int*, int, int);

typedef struct search_args {
    int *arr;
    int expected;
    int thread_id;
    int start; 
    int end;
    int *result;
} search_args;

int main(void) {
    srand(time(NULL));

    pthread_t threads[num_threads];
    int arr[items];
    generate_arr(arr, items, 101);

    int found = 0;

    for (int id = 0; id < num_threads; id++) {
        int items_per_thread = items/num_threads;
        int remaining = items % num_threads;

        int start = (id * items_per_thread) + min(id, remaining);
        int end = start + items_per_thread + (id < remaining ? 1 : 0);

        search_args *args = (search_args*) malloc(sizeof(search_args));
        
        args -> arr = arr;
        args -> expected = 2;
        args -> thread_id = id;
        args -> start = start;
        args -> end = end;
        args -> result = &found;
        
        int code = pthread_create(&threads[id], NULL, search, (void*) args);
        if (code) {
            printf("Erro ao criar a thread %d\n", id);
            exit(-1);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Valor encontrado? %d\n", found);

    return 0;
}

void* search(void* args) {
    search_args* arg = (search_args*) args;

    int *arr = arg -> arr;
    int *result = arg -> result;
    int id = arg -> thread_id;
    int start = arg -> start;
    int end = arg -> end;
    int expected = arg -> expected;

    for (int i = start; i < end; i++) {
        if (arr[i] == expected) {
            *(result) = 1;
            printf("(Thread #%d) Encontrado o valor %d no index %d!\n", id, expected, i);
        }
    }

    free(args);
    
    pthread_exit(NULL);

    return NULL;
}

void generate_arr(int* arr, int size, int cap) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % cap;
    }
}