# Pingus Wii Port

This is a port of Pingus (the Lemmings clone) to the Nintendo Wii.

## Prerequisites

Before building, you need to set up the Wii development environment:

1. **devkitPPC**: Install the PowerPC development kit for Wii
   - Download from https://devkitpro.org/
   - Set the `DEVKITPPC` environment variable to point to your devkitPPC installation

2. **Wii Libraries**: Install the necessary Wii development libraries:
   - libogc (included with devkitPPC)
   - SDL-wii (SDL port for Wii)
   - SDL_mixer-wii
   - SDL_image-wii
   - libfat
   - wiikeyboard

## Building

### Step 1: Prepare the source code
```bash
# Convert boost::shared_ptr to std::shared_ptr (required for Wii)
./convert_boost.sh
```

### Step 2: Build Pingus
```bash
# Build the Wii executable
make
```

This will create:
- `pingus.elf` - The ELF executable
- `pingus.dol` - The Wii DOL file for loading

### Step 3: Prepare data files
```bash
# Copy data files to SD card format
make data
```

This creates a `sd/` directory with the game data in the correct structure for Wii SD card access.

## Running on Wii

1. Copy the `sd/` directory to your Wii's SD card
2. Copy `pingus.dol` to `sd:/apps/pingus/boot.dol`
3. Launch the homebrew channel and run Pingus

## Wii Controls

The Wii port implements the following control mappings:

### Wii Remote Controls:
- **D-pad**: Arrow keys (↑↓←→) for menu navigation and game movement
- **A Button**: Left mouse click for selecting/clicking in menus and game
- **Minus (-) Button**: Escape key for back/exit menus
- **Home Button**: Exit the game

### Supported Controllers:
- **Wii Remote**: Basic navigation and selection
- **Wii Remote + Nunchuk**: Full analog control support
- **Classic Controller**: Enhanced control with additional buttons
- **GameCube Controller**: Full controller support

### Implementation:
Controller inputs are automatically translated to SDL keyboard and mouse events, so Pingus processes them as standard input devices. The system runs Pingus in a separate thread while the main thread handles Wii controller input and injects SDL events.

## Technical Details

### Wii-Specific Changes

- **Memory Management**: Uses Wii's memory management system
- **Graphics**: SDL-based rendering adapted for Wii framebuffer
- **Input**: Wii controller support through libogc
- **Storage**: SD card access for game data
- **Audio**: SDL_mixer for sound and music

### Code Modifications

- Converted `boost::shared_ptr` to `std::shared_ptr` for C++11 compatibility
- Disabled PhysFS (not available on Wii)
- Disabled OpenGL support
- Wii-specific main entry point with hardware initialization
- Configured data paths for SD card access

### Build System

The Makefile automatically:
- Discovers all Pingus source files
- Sets Wii-specific compiler flags
- Links against Wii libraries
- Handles data file embedding
- Generates both ELF and DOL files

## Troubleshooting

### Build Issues
- Make sure `DEVKITPPC` environment variable is set
- Ensure all Wii libraries are installed
- Check that you have sufficient RAM for compilation

### Runtime Issues
- Verify SD card is properly formatted
- Ensure data files are in `sd:/apps/pingus/data/`
- Check Wii homebrew environment is set up correctly

## Project Structure

```
pingus-wii/
├── main.cpp              # Wii-specific main entry point
├── Makefile              # Main build system
├── convert_boost.sh      # Boost to std::shared_ptr conversion
├── src/                  # Pingus source code
├── data/                 # Game assets
├── application/          # Original Wii template
├── library/              # Template library
└── sd/                   # Generated SD card structure
```

## Contributing

This is a port of the original Pingus game. For Pingus-specific issues, refer to the upstream project. For Wii-specific porting issues, feel free to contribute fixes and improvements.

## License

Pingus is free software licensed under the GNU General Public License. This Wii port maintains the same license.
