#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

typedef struct {
    void** buffer;         
    size_t capacity; 
    size_t head;   
    size_t tail;  
    bool is_full; 
} Circular_Buffer;

bool empty(Circular_Buffer* cb) {
    return (!cb->is_full && (cb->head == cb->tail));
}

bool full(Circular_Buffer* cb) {
    return cb->is_full;
}

void put(Circular_Buffer* cb, void* data) {
    // adds an item to the buffer and advances the tail pointer
    cb->buffer[cb->tail] = data;
    cb->tail = (cb->tail + 1) % cb->capacity;

    if (cb->is_full) {
        cb->head = (cb->head + 1) % cb->capacity;
    }

    cb->is_full = (cb->tail == cb->head);
}

void* get(Circular_Buffer* cb) {
    // removes an item from the buffer and advances the head pointer
    if (!empty(cb)) {
        void* data = cb->buffer[cb->head];
        cb->head = (cb->head + 1) % cb->capacity;
        cb->is_full = false;
        return data;
    }
    printf("Buffer is empty, nothing to get.\n");
    return NULL;
}

void init_buffer(Circular_Buffer* cb, size_t capacity) {
    if (capacity == 0) return;

    cb->buffer = malloc(capacity * sizeof(void*));
    if (!cb->buffer) {
        fprintf(stderr, "Allocation failed\n");
        exit(EXIT_FAILURE);
    }

    cb->capacity = capacity;
    cb->head = 0;
    cb->tail = 0;
    cb->is_full = false;
}

void delete_buffer(Circular_Buffer* cb) {
    free(cb->buffer);
    cb->buffer = NULL;
    cb->capacity = cb->head = cb->tail = 0;
    cb->is_full = false;
}

void dump_buffer(Circular_Buffer* cb) {
    printf("Buffer contents (capacity = %zu):\n", cb->capacity);

    if (empty(cb)) {
        printf("Buffer is empty.\n");
        return;
    }

    size_t i = cb->head;
    while (i != cb->tail) {
        if (i == cb->head) {
            printf("head -> %d\n", *(int*)cb->buffer[i]);
        }
        else if (i == (cb->tail - 1 + cb->capacity) % cb->capacity) {
            printf("tail -> %d\n", *(int*)cb->buffer[i]);
        }
        else {
            printf("   : %d\n", *(int*)cb->buffer[i]);
        }
        i = (i + 1) % cb->capacity;
    }
    printf("---\n");
}


int main() {
    Circular_Buffer cb;
    init_buffer(&cb, 6);

    for (size_t i = 1; i < 4; i++) {
        int* value = malloc(sizeof(int));
        *value = (int)i;
        put(&cb, value);
    }

    dump_buffer(&cb);

    int* val = (int*)get(&cb);
    if (val) {
        free(val);
    }

    dump_buffer(&cb);

    delete_buffer(&cb);

    return 0;
}
