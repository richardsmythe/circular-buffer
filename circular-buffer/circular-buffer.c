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
    while (!empty(cb)) {
        free(get(cb));
    }
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

    printf("--- Test 1: Put and get ---\n");
    init_buffer(&cb, 5);
    int* a = malloc(sizeof(int)); *a = 10;
    int* b = malloc(sizeof(int)); *b = 20;
    int* c = malloc(sizeof(int)); *c = 30;
    put(&cb, a);
    put(&cb, b);
    dump_buffer(&cb);
    int* val1 = (int*)get(&cb);
    printf("Got: %d\n", *val1);
    dump_buffer(&cb);
    put(&cb, c);
    dump_buffer(&cb);
    int* val2 = (int*)get(&cb);
    printf("Got: %d\n", *val2);
    int* val3 = (int*)get(&cb);
    printf("Got: %d\n", *val3);
    dump_buffer(&cb);
    assert(*val1 == 10);
    assert(*val2 == 20);
    assert(*val3 == 30);
    assert(empty(&cb));
    delete_buffer(&cb);

    printf("\n--- Test 2: Filling and emptying the buffer ---\n");
    init_buffer(&cb, 5);
    int* d = malloc(sizeof(int)); *d = 40;
    int* e = malloc(sizeof(int)); *e = 50;
    int* f = malloc(sizeof(int)); *f = 60;
    int* g = malloc(sizeof(int)); *g = 70;
    int* h = malloc(sizeof(int)); *h = 80;
    put(&cb, d);
    put(&cb, e);
    put(&cb, f);
    put(&cb, g);
    put(&cb, h);
    dump_buffer(&cb);
    assert(full(&cb));
    assert(*(int*)cb.buffer[0] == 40);
    assert(*(int*)cb.buffer[1] == 50);
    assert(*(int*)cb.buffer[2] == 60);
    assert(*(int*)cb.buffer[3] == 70);
    assert(*(int*)cb.buffer[4] == 80);

    int* r1 = (int*)get(&cb);
    int* r2 = (int*)get(&cb);
    printf("Got: %d, %d\n", *r1, *r2);
    dump_buffer(&cb);
    assert(*r1 == 40);
    assert(*r2 == 50);
    assert(!full(&cb));
    assert(!empty(&cb));

    int* aa = malloc(sizeof(int)); *aa = 10;
    int* bb = malloc(sizeof(int)); *bb = 20;
    put(&cb, aa);
    put(&cb, bb);
    dump_buffer(&cb);
    assert(*(int*)cb.buffer[2] == 60);
    assert(*(int*)cb.buffer[3] == 70);
    assert(*(int*)cb.buffer[4] == 80);
    assert(*(int*)cb.buffer[0] == 10);
    assert(*(int*)cb.buffer[1] == 20);
    assert(full(&cb));
    delete_buffer(&cb);    

    printf("\n--- Test 3: Empty buffer operations ---\n");
    init_buffer(&cb, 3);
    assert(empty(&cb));
    assert(get(&cb) == NULL);
    dump_buffer(&cb);
    delete_buffer(&cb);

    printf("\n--- Test 4: Single element buffer ---\n");
    init_buffer(&cb, 1);
    int* o = malloc(sizeof(int)); *o = 100;
    int* p = malloc(sizeof(int)); *p = 10;
    put(&cb, o);
    assert(full(&cb));
    dump_buffer(&cb);
    int* qq = (int*)get(&cb);
    assert(*qq == 100);
    assert(empty(&cb));
    put(&cb, p);
    dump_buffer(&cb);
    int* rr = (int*)get(&cb);
    assert(*rr == 10);
    assert(empty(&cb));
    delete_buffer(&cb);

    return 0;
}