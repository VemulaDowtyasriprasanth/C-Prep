#include <iostream>

void memoryLeakExample() {
    int* leakedArray = new int[100]; // Dynamically allocated array

    // Simulate some operations
    for (int i = 0; i < 100; ++i) {
        leakedArray[i] = i * 2;
    }

    // Oops! Forgot to free the memory
    // delete[] leakedArray;  // This line is missing
}

int main() {
    memoryLeakExample();
    return 0;
}


/*

Detecting the Leak with Valgrind
Compile with Debug Information
To make debugging easier, compile the code with debugging symbols enabled:

bash
Copy code
g++ -g -o memory_leak_example memory_leak_example.cpp
Run Valgrind
Execute the program under Valgrind to check for memory leaks:

bash
Copy code
valgrind --leak-check=full --track-origins=yes ./memory_leak_example
Analyze Valgrind Output
Valgrind will report something like this:

csharp
Copy code
==12345== 400 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x4C2B975: operator new[](unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==12345==    by 0x40118A: memoryLeakExample() (memory_leak_example.cpp:4)
==12345==    by 0x4011B6: main (memory_leak_example.cpp:12)
Key Details:
400 bytes were leaked (100 * sizeof(int)).
The allocation occurred in memoryLeakExample() at line 4.
The memory was not deallocated (no corresponding delete[]).
Fixing the Memory Leak
To resolve the memory leak, we ensure that every dynamically allocated block of memory is properly deallocated. Modify the function as follows:

Fixed Code:
cpp
Copy code
void memoryLeakExample() {
    int* leakedArray = new int[100]; // Dynamically allocated array

    for (int i = 0; i < 100; ++i) {
        leakedArray[i] = i * 2;
    }

    delete[] leakedArray;  // Properly free the memory
}
Re-run Valgrind to Verify
After fixing the code, run Valgrind again:

bash
Copy code
valgrind --leak-check=full --track-origins=yes ./memory_leak_example
Expected Output:

sql
Copy code
==12345== All heap blocks were freed -- no leaks are possible
==12345== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
Explanation (Interview-Style)
**"To detect a memory leak, I use tools like Valgrind, which tracks memory allocations and detects leaks. In the provided example, Valgrind flagged a leak of 400 bytes, pointing to a missing delete[] for a dynamically allocated array. By analyzing the stack trace provided by Valgrind, I traced the issue to the memoryLeakExample function.

The resolution involved adding a delete[] statement to ensure the memory was properly freed after use. After applying the fix, I re-ran Valgrind to verify that all allocated memory was released, confirming the issue was resolved.

Additionally, in real-world scenarios, I advocate for using modern C++ practices like smart pointers (std::unique_ptr or std::shared_ptr), which automate memory management and prevent such leaks altogether."**

Let me know if you’d like to explore another debugging scenario!



*/