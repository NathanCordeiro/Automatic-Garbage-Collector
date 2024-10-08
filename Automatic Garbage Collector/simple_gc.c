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
    heap->compactionPtr = heap->heapStart;
}

void freeGCHeap(GCHeap* heap) {
    heap->stackSize = 0;
    heap->numValues = 0;
    heap->firstValue = NULL;
    free(heap->heapStart);
    heap->heapStart = heap->heapEnd = heap->freePtr = NULL;
}

GCValue* allocateValue(GCHeap* heap, ValueType type) {
    if (heap->numValues == heap->maxValues) {
        collectGarbage(heap);
    }

    // Allocate space for the GCValue
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
            free(unreached);  // Free the memory occupied by the unreached object
        } else {
            (*value)->marked = 0;  // Reset the mark for the next GC cycle
            value = &(*value)->next;
        }
    }
}

void collectGarbage(GCHeap* heap) {
    size_t numValues = heap->numValues;
    markAllValues(heap);
    sweepUnmarkedValues(heap);
    compactMemory(heap);
    heap->maxValues = heap->numValues * 2;

    printf("Collected %zu values, %zu remaining.\n", numValues - heap->numValues, heap->numValues);
}

void compactMemory(GCHeap* heap) {
    GCValue* current = heap->firstValue;
    uint8_t* newFreePtr = heap->heapStart;
    GCValue* prev = NULL;

    // Move live objects to the beginning of the heap
    while (current) {
        if (current->marked) {
            size_t size = sizeof(GCValue);
            if (newFreePtr + size > heap->heapEnd) {
                fprintf(stderr, "Out of memory during compaction\n");
                exit(1);
            }

            // Move the object to the new location
            GCValue* newLocation = (GCValue*)newFreePtr;
            memcpy(newLocation, current, size);
            newFreePtr += size;

            // Update linked list pointers
            if (prev) {
                prev->next = newLocation;
            } else {
                heap->firstValue = newLocation;
            }
            newLocation->next = NULL;

            // Update stack pointers
            for (int i = 0; i < heap->stackSize; i++) {
                if (heap->stack[i] == current) {
                    heap->stack[i] = newLocation;
                }
            }

            prev = newLocation;
        }
        current = current->next;
    }

    // Update freePtr
    heap->freePtr = newFreePtr;
}


// Creation functions
GCValue* createIntValue(GCHeap* heap, int value) {
    GCValue* gcValue = allocateValue(heap, VALUE_INT);
    gcValue->intValue = value;
    return gcValue;
}

GCValue* createFloatValue(GCHeap* heap, float value) {
    GCValue* gcValue = allocateValue(heap, VALUE_FLOAT);
    gcValue->floatValue = value;
    return gcValue;
}

GCValue* createDoubleValue(GCHeap* heap, double value) {
    GCValue* gcValue = allocateValue(heap, VALUE_DOUBLE);
    gcValue->doubleValue = value;
    return gcValue;
}

GCValue* createCharValue(GCHeap* heap, char value) {
    GCValue* gcValue = allocateValue(heap, VALUE_CHAR);
    gcValue->charValue = value;
    return gcValue;
}

GCValue* createPairValue(GCHeap* heap, GCValue* head, GCValue* tail) {
    GCValue* gcValue = allocateValue(heap, VALUE_PAIR);
    gcValue->pairValue.head = head;
    gcValue->pairValue.tail = tail;
    return gcValue;
}

GCValue* createEnumValue(GCHeap* heap, int enumValue) {
    GCValue* gcValue = allocateValue(heap, VALUE_ENUM);
    gcValue->enumValue = enumValue;
    return gcValue;
}

GCValue* createUnionValue(GCHeap* heap, int tag, void* data) {
    GCValue* gcValue = allocateValue(heap, VALUE_UNION);
    gcValue->unionValue.tag = tag;
    switch (tag) {
        case 0: gcValue->unionValue.data.intMember = *(int*)data; break;
        case 1: gcValue->unionValue.data.floatMember = *(float*)data; break;
        case 2: gcValue->unionValue.data.doubleMember = *(double*)data; break;
        case 3: gcValue->unionValue.data.charMember = *(char*)data; break;
        default: fprintf(stderr, "Invalid tag for union\n"); exit(1);
    }
    return gcValue;
}

// Type conversion functions
GCValue* convertToInt(GCHeap* heap, GCValue* value) {
    if (checkNullPointer(value) || !checkTypeConversion(value, VALUE_INT)) return NULL;

    int result;
    switch (value->type) {
        case VALUE_INT: return value;
        case VALUE_FLOAT: result = (int)value->floatValue; break;
        case VALUE_DOUBLE: result = (int)value->doubleValue; break;
        case VALUE_CHAR: result = (int)value->charValue; break;
        default: return NULL; // Conversion not supported
    }
    return createIntValue(heap, result);
}

GCValue* convertToFloat(GCHeap* heap, GCValue* value) {
    if (checkNullPointer(value) || !checkTypeConversion(value, VALUE_FLOAT)) return NULL;

    float result;
    switch (value->type) {
        case VALUE_FLOAT: return value;
        case VALUE_INT: result = (float)value->intValue; break;
        case VALUE_DOUBLE: result = (float)value->doubleValue; break;
        case VALUE_CHAR: result = (float)value->charValue; break;
        default: return NULL; // Conversion not supported
    }
    return createFloatValue(heap, result);
}

GCValue* convertToDouble(GCHeap* heap, GCValue* value) {
    if (checkNullPointer(value) || !checkTypeConversion(value, VALUE_DOUBLE)) return NULL;

    double result;
    switch (value->type) {
        case VALUE_DOUBLE: return value;
        case VALUE_INT: result = (double)value->intValue; break;
        case VALUE_FLOAT: result = (double)value->floatValue; break;
        case VALUE_CHAR: result = (double)value->charValue; break;
        default: return NULL; // Conversion not supported
    }
    return createDoubleValue(heap, result);
}

GCValue* convertToChar(GCHeap* heap, GCValue* value) {
    if (checkNullPointer(value) || !checkTypeConversion(value, VALUE_CHAR)) return NULL;

    char result;
    switch (value->type) {
        case VALUE_CHAR: return value;
        case VALUE_INT: result = (char)value->intValue; break;
        case VALUE_FLOAT: result = (char)value->floatValue; break;
        case VALUE_DOUBLE: result = (char)value->doubleValue; break;
        default: return NULL; // Conversion not supported
    }
    return createCharValue(heap, result);
}

// Error handling functions
int checkNullPointer(GCValue* value) {
    if (value == NULL) {
        fprintf(stderr, "Error: NULL pointer\n");
        return 1;
    }
    return 0;
}

int checkTypeConversion(GCValue* value, ValueType expectedType) {
    if (value->type == expectedType) {
        return 1;
    } else {
        fprintf(stderr, "Error: Type conversion from %d to %d not supported\n", value->type, expectedType);
        return 0;
    }
}
