#!/bin/bash

echo "Adding memory header and fixing surface.hpp..."

# Add memory header after string
sed -i '/#include <string>/a #include <memory>' src/display/surface.hpp

echo "Fixed! Now recompiling..."
