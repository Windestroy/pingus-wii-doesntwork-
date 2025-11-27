#!/bin/bash

echo "Finding and fixing all missing cassert includes..."

# Find all files that use assert but don't include cassert
grep -r "assert(" src/ --include="*.cpp" -l | while read file; do
    # Check if the file already includes cassert
    if ! grep -q "#include <cassert>" "$file"; then
        echo "Fixing: $file"
        # Add cassert include after the first #include line
        sed -i '0,/#include/s/#include.*/#include <cassert>\n&/' "$file"
    fi
done

echo "Done! Trying to compile..."
