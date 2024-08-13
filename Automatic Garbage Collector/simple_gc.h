#ifndef SIMPLE_GC_H
#define SIMPLE_GC_H

#include <stddef.h>
#include <stdint.h>

#define HEAP_SIZE 1024 * 1024  // 1MB heap
#define STACK_MAX 256

typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_DOUBLE,
    VALUE_CHAR,
    VALUE_PAIR,
    VALUE_ENUM,
    VALUE_UNION
} ValueType;

typedef struct GCValue {
    ValueType type;
    unsigned char marked;
    struct GCValue* next;
    union {
        int intValue;
        float floatValue;
        double doubleValue;
        char charValue;
        struct {
            struct GCValue* head;
            struct GCValue* tail;
        } pairValue;
        int enumValue;
        struct {
            int tag;
            union {
                int intMember;
                float floatMember;
                double doubleMember;
                char charMember;
            } data;
        } unionValue;
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

// Creation functions
GCValue* createIntValue(GCHeap* heap, int value);
GCValue* createFloatValue(GCHeap* heap, float value);
GCValue* createDoubleValue(GCHeap* heap, double value);
GCValue* createCharValue(GCHeap* heap, char value);
GCValue* createPairValue(GCHeap* heap, GCValue* head, GCValue* tail);
GCValue* createEnumValue(GCHeap* heap, int enumValue);
GCValue* createUnionValue(GCHeap* heap, int tag, void* data);

// Type conversion functions
GCValue* convertToInt(GCHeap* heap, GCValue* value);
GCValue* convertToFloat(GCHeap* heap, GCValue* value);
GCValue* convertToDouble(GCHeap* heap, GCValue* value);
GCValue* convertToChar(GCHeap* heap, GCValue* value);

#endif // SIMPLE_GC_H
