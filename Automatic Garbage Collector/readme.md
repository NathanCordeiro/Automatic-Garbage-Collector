## Syntax and Data Types:

1. GCValue Structure:
The core of the garbage collector is the GCValue structure. It's defined like this:
```c
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
```

This structure represents any value managed by the garbage collector. The `type` field indicates what kind of value it is (integer or pair), and the `marked` field is used during garbage collection.

2. GCValue* as the Universal Pointer:
Almost all data types are represented as GCValue*. This is a common approach in garbage-collected systems, where all managed objects are accessed through a single pointer type.

3. Creating Values:
To create new values, you use functions like:
```c
GCValue* intValue = createIntValue(&heap, 42);
GCValue* floatValue = createFloatValue(&heap, 3.14f);
GCValue* pairValue = createPairValue(&heap, intValue, floatValue);

GCValue* createIntValue(GCHeap* heap, int value);
GCValue* createPairValue(GCHeap* heap, GCValue* head, GCValue* tail);
```
These functions allocate a new GCValue and set it up with the appropriate type and data.

4. Accessing Values:
To access the data in a GCValue, you need to check its type and then access the appropriate union member:
```c
if (value->type == VALUE_INT) {
    int x = value->intValue;
} else if (value->type == VALUE_PAIR) {
    GCValue* head = value->pairValue.head;
    GCValue* tail = value->pairValue.tail;
}
```

5. To convert between types:
```c
GCValue* convertedValue = convertToInt(&heap, floatValue);
```

6. Memory Management:
The `GCHeap` structure manages the memory:
```c
typedef struct {
    GCValue* stack[STACK_MAX];
    int stackSize;
    GCValue* firstValue;
    size_t numValues;
    size_t maxValues;
    uint8_t* heapStart;
    uint8_t* heapEnd;
    uint8_t* freePtr;
    uint8_t* compactionPtr;
} GCHeap;
```
You initialize it with `initGCHeap(&heap)` and free it with `freeGCHeap(&heap)`.
- stack: A stack of GCValue* pointers to protect values from garbage collection.
- stackSize: The current size of the stack.
- firstValue: Points to the first GCValue in the managed list.
- numValues: The number of values currently managed.
- maxValues: The maximum number of values before triggering garbage collection.
- heapStart, heapEnd, freePtr: Pointers managing the start, end, and current free space of the heap.

7. Stack Operations:
To protect values from being collected, you use:
```c
void pushValue(GCHeap* heap, GCValue* value);
GCValue* popValue(GCHeap* heap);
```

8. Garbage Collection:
Garbage collection is triggered automatically when needed, but you can also call it manually:
```c
void collectGarbage(GCHeap* heap);
```


## Key Points:

- All managed values are of type `GCValue*`.
- You create values using `createIntValue` and `createPairValue` and so on.
- You access values by checking their type and then accessing the appropriate union member.
- You protect values from collection by pushing them onto the stack with pushValue.
- You let the garbage collector know you're done with a value by popping it from the stack with `popValue`.
- Supports multiple data types like int, float, double, char, pair, enum, and union.
- Mark-and-sweep algorithm for garbage collection.
- Memory compaction to reduce fragmentation.
- Type conversion functions for safe type casting.
- Error handling for null pointers and invalid type conversions.

## Memory Management:
The garbage collector uses a mark-and-sweep algorithm with memory compaction:

- Marking: All reachable objects are marked by traversing from the root set (stack).
- Sweeping: Unmarked objects are freed.
- Compaction: Live objects are moved to the beginning of the heap to reduce fragmentation.

## Error Handling:
The library includes functions for handling common errors:

- `checkNullPointer`: Checks for null pointers.
- `checkTypeConversion`: Validates type conversions.

This design means that instead of using native C types directly, you're always working with `GCValue*` pointers. This allows the garbage collector to manage all the memory for you, but it does require you to use the provided functions to create and manipulate values.




