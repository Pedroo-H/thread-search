#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

/*
    TO-DO:
        - Paralelizar geração aleátoria de números
        - Criar comparativo de com thread vs sem thread
        - Organizar código
*/

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

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 4) {
        printf("Argumentos insuficientes!\n");
        exit(-1);
    }

    int num_threads = atoi(argv[1]);
    int items = atoi(argv[2]);
    int expected = atoi(argv[3]);

    pthread_t threads[num_threads];
    int arr[items];
    generate_arr(arr, items, 101);

    int found = 0;

    clock_t start = clock();

    for (int id = 0; id < num_threads; id++) {
        int items_per_thread = items/num_threads;
        int remaining = items % num_threads;

        int start = (id * items_per_thread) + min(id, remaining);
        int end = start + items_per_thread + (id < remaining ? 1 : 0);

        search_args *args = (search_args*) malloc(sizeof(search_args));
        
        args -> arr = arr;
        args -> expected = expected;
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

    clock_t end = clock();
    printf("%lf (ms)\n", (double) (end-start)/CLOCKS_PER_SEC);

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