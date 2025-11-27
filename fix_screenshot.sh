#!/bin/bash

echo "Fixing screenshot.cpp for newer libpng..."

# Backup the file
cp src/screenshot.cpp src/screenshot.cpp.bak

# Replace the old setjmp usage with the new API
sed -i 's/setjmp(png_ptr->jmpbuf)/setjmp(png_jmpbuf(png_ptr))/g' src/screenshot.cpp

echo "Fixed screenshot.cpp"
echo "Running make..."
