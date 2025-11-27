#!/bin/bash

echo "Fixing surface.cpp and surface.hpp..."

# Fix surface.hpp - add missing includes
sed -i '1a #include <string>\n#include "SDL.h"' src/display/surface.hpp

# Fix "operate" typo - should be "operator"
sed -i 's/operate bool()/operator bool()/g' src/display/surface.hpp

# Fix SharedPtr - should be std::shared_ptr (we converted boost)
sed -i 's/SharedPtr</std::shared_ptr</g' src/display/surface.hpp

# Fix surface.cpp - add missing includes
sed -i '21a #include <string>\n#include <iostream>\n#include "SDL_image.h"' src/display/surface.cpp

# Fix the operator typo in cpp file too
sed -i 's/Surface::operate bool()/Surface::operator bool()/g' src/display/surface.cpp

echo "Fixed surface files"
