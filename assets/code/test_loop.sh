#!/bin/bash

# 1. Define your array of buffer sizes here (separated by spaces)
BUFFER_SIZES=(10 64 128 256 512 1024 4096 16384 65536 262144 1048576 2097152 4194403 8388606 16777216 33554432 67108864 134217728)

# 2. Define the source file
SOURCE_FILE="reptester/reptest_main.c"

# Loop through each size in the array
for size in "${BUFFER_SIZES[@]}"; do
    echo "=================================================="
    echo "🚀 Compiling and running with BUFFER_SIZE = $size"
    echo "=================================================="

    # Compile the code with the dynamic buffer size
    gcc -Wall -Wextra -DBUFFER_SIZE=$size -DENABLE_PROFILER=1 $SOURCE_FILE

    # Check if compilation was successful ($? holds the exit status of the last command)
    if [ $? -eq 0 ]; then
        # Run the executable
        sudo ./a.out
    else
        echo "❌ Compilation failed for BUFFER_SIZE = $size. Aborting."
        exit 1
    fi

    echo "" # Add a blank line for visual spacing in the terminal
done

echo "✅ All benchmark runs completed successfully!"
