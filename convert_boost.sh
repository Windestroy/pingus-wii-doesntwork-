#!/bin/bash
# Script to convert boost::shared_ptr to std::shared_ptr in Pingus source

echo "Converting boost::shared_ptr to std::shared_ptr..."

# Find all .cpp and .hpp files
find src -name "*.cpp" -o -name "*.hpp" | while read file; do
    # Replace boost::shared_ptr with std::shared_ptr
    sed -i 's/boost::shared_ptr/std::shared_ptr/g' "$file"
    
    # Replace #include <boost/shared_ptr.hpp> with #include <memory>
    sed -i 's/#include <boost\/shared_ptr.hpp>/#include <memory>/g' "$file"
    
    echo "Processed: $file"
done

echo "Conversion complete!"
echo ""
echo "Creating config.h for Wii..."

# Create a basic config.h for Wii
cat > src/config.h << 'CONFIGEOF'
#ifndef CONFIG_H
#define CONFIG_H

// Wii-specific configuration
#define GEKKO 1
#define WII 1

// Disable features not available on Wii
#undef HAVE_OPENGL
#define HAVE_SDL 1

// PhysFS support (we may need to disable this)
#define HAVE_PHYSFS 0

// Disable binreloc
#undef ENABLE_BINRELOC

// Paths
#define PINGUS_DATADIR "sd:/apps/pingus/data"

#endif // CONFIG_H
CONFIGEOF

echo "Created src/config.h"
