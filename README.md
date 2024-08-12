# Automatic-Garbage-Collector : HeapSweep
---
For most C programs, manual memory management with malloc/free remains the standard approach. Garbage collection is more commonly used in higher-level languages 
like Java, Python, or Go, where the benefits of automatic memory management often outweigh the costs.

## License
[![Licence](https://img.shields.io/github/license/Ileriayo/markdown-badges?style=for-the-badge)](./LICENSE)


## Using the GC in your project

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





