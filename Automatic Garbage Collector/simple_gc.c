#include "simple_gc.h"
#include <stdlib.h>
#include <stdio.h>

void initGCHeap(GCHeap* heap) {
    heap->stackSize = 0;
    heap->firstValue = NULL;
    heap->numValues = 0;
    heap->maxValues = 8;
    heap->heapStart = (uint8_t*)malloc(HEAP_SIZE);
    heap->heapEnd = heap->heapStart + HEAP_SIZE;
    heap->freePtr = heap->heapStart;
}

void freeGCHeap(GCHeap* heap) {
    heap->stackSize = 0;
    heap->numValues = 0;
    heap->firstValue = NULL;
    free(heap->heapStart);
    heap->heapStart = heap->heapEnd = heap->freePtr = NULL;
}

GCValue* allocateValue(GCHeap* heap, ValueType type) {
    if (heap->numValues == heap->maxValues) collectGarbage(heap);

    GCValue* value = (GCValue*)heap->freePtr;
    heap->freePtr += sizeof(GCValue);

    if (heap->freePtr > heap->heapEnd) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    value->type = type;
    value->marked = 0;
    value->next = heap->firstValue;
    heap->firstValue = value;
    heap->numValues++;

    return value;
}

void pushValue(GCHeap* heap, GCValue* value) {
    if (heap->stackSize >= STACK_MAX) {
        fprintf(stderr, "Stack overflow\n");
        exit(1);
    }
    heap->stack[heap->stackSize++] = value;
}

GCValue* popValue(GCHeap* heap) {
    if (heap->stackSize == 0) {
        fprintf(stderr, "Stack underflow\n");
        exit(1);
    }
    return heap->stack[--heap->stackSize];
}

void markAllValues(GCHeap* heap) {
    for (int i = 0; i < heap->stackSize; i++) {
        GCValue* value = heap->stack[i];
        if (value && !value->marked) {
            value->marked = 1;
            if (value->type == VALUE_PAIR) {
                if (value->pairValue.head) value->pairValue.head->marked = 1;
                if (value->pairValue.tail) value->pairValue.tail->marked = 1;
            }
        }
    }
}

void sweepUnmarkedValues(GCHeap* heap) {
    GCValue** value = &heap->firstValue;
    while (*value) {
        if (!(*value)->marked) {
            GCValue* unreached = *value;
            *value = unreached->next;
            heap->numValues--;
        } else {
            (*value)->marked = 0;
            value = &(*value)->next;
        }
    }
}

void collectGarbage(GCHeap* heap) {
    size_t numValues = heap->numValues;
    markAllValues(heap);
    sweepUnmarkedValues(heap);
    heap->maxValues = heap->numValues * 2;

    printf("Collected %zu values, %zu remaining.\n", numValues - heap->numValues, heap->numValues);
}

GCValue* createIntValue(GCHeap* heap, int value) {
    GCValue* gcValue = allocateValue(heap, VALUE_INT);
    gcValue->intValue = value;
    return gcValue;
}

GCValue* createPairValue(GCHeap* heap, GCValue* head, GCValue* tail) {
    GCValue* gcValue = allocateValue(heap, VALUE_PAIR);
    gcValue->pairValue.head = head;
    gcValue->pairValue.tail = tail;
    return gcValue;
}
