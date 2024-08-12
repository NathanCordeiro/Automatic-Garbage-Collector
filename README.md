# Automatic-Garbage-Collector : HeapSweep
---
For most C programs, manual memory management with malloc/free remains the standard approach. Garbage collection is more commonly used in higher-level languages 
like Java, Python, or Go, where the benefits of automatic memory management often outweigh the costs.

The main motivation for creating a garbage collector is to simplify memory management in C programs. C is a powerful language, but it requires manual memory management, which can lead to issues like memory leaks, dangling pointers, and double frees. These problems can be difficult to debug and can cause program crashes or unpredictable behavior.

While not suitable for production use, this project serves as an excellent learning tool for understanding garbage collection concepts and how they can be implemented in a low-level language like C.

The implementation is intentionally kept simple to make it easy to understand and modify. This makes it a good starting point for further experimentation or for educational purposes. 

## License
[![Licence](https://img.shields.io/github/license/Ileriayo/markdown-badges?style=for-the-badge)](./LICENSE)


## Comparison of Memory Management Methods

| **Aspect**                                | **Manual Memory Management (malloc/free)**                                                                 | **Reference Counting**                                                                                      | **Region-Based Memory Management**                                                                         | **HeapSweep**                                                                 |
|-------------------------------------------|------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------|
| **Memory Allocation/Deallocation**        | Programmer explicitly allocates and frees memory                                                           | Each allocation has a count of references to it                                                            | Allocations are grouped into regions                                                                       | Automatically reclaims memory that's no longer in use                                           |
| **Error Prone**                           | Prone to memory leaks, use-after-free, and double-free errors                                              | Can't handle cyclic references without additional mechanisms                                               | Can lead to holding onto memory longer than necessary                                                      | Prevents common memory-related errors                                                          |
| **Control**                               | Gives fine-grained control over memory usage                                                               | -                                                                                                          | -                                                                                                          | Less control over exact timing of memory deallocation                                          |
| **Runtime Overhead**                      | No runtime overhead for tracking objects                                                                   | Overhead for maintaining counts                                                                            | -                                                                                                          | Adds some runtime overhead for tracking and collecting garbage                                 |
| **Cyclic References**                     | -                                                                                                          | Can't handle cyclic references                                                                             | -                                                                                                          | Can handle cyclic references                                                                   |
| **Reference Counts**                      | -                                                                                                          | Memory is freed when reference count drops to zero                                                         | -                                                                                                          | No need to update reference counts                                                             |
| **Bulk Allocations/Deallocations**        | -                                                                                                          | -                                                                                                          | Entire regions are freed at once                                                                           | May be less efficient for bulk allocations/deallocations                                       |
| **Efficiency**                            | -                                                                                                          | -                                                                                                          | Efficient for certain patterns of allocation/deallocation                                                  | Potentially more efficient for highly interconnected data structures                           |
| **Flexibility**                           | -                                                                                                          | -                                                                                                          | -                                                                                                          | More flexible for general-purpose use                                                          |


## Using HeapSweep in your project

1. Include the files:
   Add `simple_gc.h` and `simple_gc.c` to your project.

2. In your main C file, include the header:
   ```c
   #include "simple_gc.h"
   ```
3. Initialize the GC heap at the start of your program:
   ```c
   GCHeap heap;
   initGCHeap(&heap);
   ```

4. Instead of using malloc for data that should be garbage collected, use the GC functions:
   ```c
   // Instead of: int* x = malloc(sizeof(int));
   GCValue* x = createIntValue(&heap, 42);

   // Instead of: struct pair* p = malloc(sizeof(struct pair));
   GCValue* p = createPairValue(&heap, x, NULL);
   ```

5. Use `pushValue` and `popValue` to manage the root set (e.g., for local variables that shouldn't be collected):
   ```c
   pushValue(&heap, x);
   // ... do some work ...
   popValue(&heap);
   ```

6. You don't need to call free on GC-managed values. The GC will handle it.

7. You can manually trigger garbage collection if needed:
   ```c
   collectGarbage(&heap);
   ```

8. Clean up at the end of your program:
   ```c
   freeGCHeap(&heap);
   ```



## Advantages and Disadvantages of HeapSweep

Benefits:

- Automatic memory management reduces cognitive load on programmers
- Helps prevent memory leaks and use-after-free bugs
- Can handle complex data structures with cyclic references
- Allows for more natural expression of certain algorithms

Drawbacks:

- Added runtime overhead for GC operations
- Less predictable memory usage and deallocation timing
- May not be suitable for real-time or memory-constrained systems



## Some examples of HeapSweep

`Factorial of a number:`
```c
#include "simple_gc.h"
#include <stdio.h>

GCValue* factorial(GCHeap* heap, int n) {
    if (n <= 1) {
        return createIntValue(heap, 1);
    }
    
    GCValue* n_minus_one = factorial(heap, n - 1);
    pushValue(heap, n_minus_one);  // Protect from GC
    
    GCValue* result = createIntValue(heap, n * n_minus_one->intValue);
    popValue(heap);  // n_minus_one no longer needed
    
    return result;
}

int main() {
    GCHeap heap;
    initGCHeap(&heap);

    int n = 5;
    GCValue* result = factorial(&heap, n);
    printf("Factorial of %d is %d\n", n, result->intValue);

    collectGarbage(&heap);
    freeGCHeap(&heap);
    return 0;
}
```

`Pointer demonstration creating array like structures:`
```c
#include "simple_gc.h"
#include <stdio.h>

void swapValues(GCHeap* heap, GCValue** a, GCValue** b) {
    GCValue* temp = *a;
    *a = *b;
    *b = temp;
}

int main() {
    GCHeap heap;
    initGCHeap(&heap);

    GCValue* value1 = createIntValue(&heap, 10);
    GCValue* value2 = createIntValue(&heap, 20);

    pushValue(&heap, value1);
    pushValue(&heap, value2);

    printf("Before swap: value1 = %d, value2 = %d\n", value1->intValue, value2->intValue);

    swapValues(&heap, &value1, &value2);

    printf("After swap: value1 = %d, value2 = %d\n", value1->intValue, value2->intValue);

    // Create a pointer to a pointer (simulating a 2D array)
    GCValue* array[2];
    array[0] = createPairValue(&heap, createIntValue(&heap, 1), createIntValue(&heap, 2));
    array[1] = createPairValue(&heap, createIntValue(&heap, 3), createIntValue(&heap, 4));

    pushValue(&heap, array[0]);
    pushValue(&heap, array[1]);

    printf("2D array: [%d, %d], [%d, %d]\n",
           array[0]->pairValue.head->intValue, array[0]->pairValue.tail->intValue,
           array[1]->pairValue.head->intValue, array[1]->pairValue.tail->intValue);

    collectGarbage(&heap);
    freeGCHeap(&heap);
    return 0;
}
```

`Linked list demonstrating creating and manipulating complex data structures:`
```c
#include "simple_gc.h"
#include <stdio.h>

void printList(GCValue* head) {
    GCValue* current = head;
    while (current != NULL && current->type == VALUE_PAIR) {
        printf("%d -> ", current->pairValue.head->intValue);
        current = current->pairValue.tail;
    }
    printf("NULL\n");
}

int main() {
    GCHeap heap;
    initGCHeap(&heap);

    // Create a linked list: 1 -> 2 -> 3 -> 4 -> 5
    GCValue* list = NULL;
    for (int i = 5; i >= 1; i--) {
        GCValue* newNode = createPairValue(&heap, createIntValue(&heap, i), list);
        pushValue(&heap, newNode);  // Protect from GC
        list = newNode;
    }

    printf("Original list: ");
    printList(list);

    // Remove the second element (2)
    if (list != NULL && list->pairValue.tail != NULL) {
        list->pairValue.tail = list->pairValue.tail->pairValue.tail;
    }

    printf("List after removing second element: ");
    printList(list);

    collectGarbage(&heap);
    freeGCHeap(&heap);
    return 0;
}
```



