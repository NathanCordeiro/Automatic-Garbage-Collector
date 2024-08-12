#ifndef SIMPLE_GC_H
#define SIMPLE_GC_H

#include <stddef.h>
#include <stdint.h>

#define HEAP_SIZE 1024 * 1024  // 1MB heap
#define STACK_MAX 256

typedef enum {
    VALUE_INT,
    VALUE_PAIR
} ValueType;

typedef struct GCValue {
    ValueType type;
    unsigned char marked;
    struct GCValue* next;
    union {
        // VALUE_INT
        int intValue;
        // VALUE_PAIR
        struct {
            struct GCValue* head;
            struct GCValue* tail;
        } pairValue;
    };
} GCValue;

typedef struct {
    GCValue* stack[STACK_MAX];
    int stackSize;

    GCValue* firstValue;
    size_t numValues;
    size_t maxValues;

    uint8_t* heapStart;
    uint8_t* heapEnd;
    uint8_t* freePtr;
} GCHeap;

void initGCHeap(GCHeap* heap);
void freeGCHeap(GCHeap* heap);
GCValue* allocateValue(GCHeap* heap, ValueType type);
void pushValue(GCHeap* heap, GCValue* value);
GCValue* popValue(GCHeap* heap);
void markAllValues(GCHeap* heap);
void sweepUnmarkedValues(GCHeap* heap);
void collectGarbage(GCHeap* heap);

// Helper functions for creating specific value types
GCValue* createIntValue(GCHeap* heap, int value);
GCValue* createPairValue(GCHeap* heap, GCValue* head, GCValue* tail);

#endif // SIMPLE_GC_H
