# Automatic-Garbage-Collector : HeapSweep
---
For most C programs, manual memory management with malloc/free remains the standard approach. Garbage collection is more commonly used in higher-level languages 
like Java, Python, or Go, where the benefits of automatic memory management often outweigh the costs.

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

