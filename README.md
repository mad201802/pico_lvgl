# Raspberry Pi Pico with LVGL and ST7789 Display

This project demonstrates how to use the LVGL (Light and Versatile Graphics Library) with an ST7789 LCD display on a Raspberry Pi Pico board. It includes a simple UI with button controls and demonstrates proper hardware integration.

## Features

- **LVGL Integration**: Full graphics library support for creating modern UIs
- **ST7789 Driver**: Hardware-accelerated SPI display driver
- **Button Support**: Debounced hardware button implementation
- **USB Serial Output**: Debug messages via USB CDC
- **Optimized for Pico**: Memory-efficient configuration for RP2040

## Hardware Requirements

- Raspberry Pi Pico (or Pico W)
- ST7789 LCD Display (240x240 recommended)
- 4 Push buttons (optional, for demo UI)
- Jumper wires and breadboard

## Pin Configuration

The default pin configuration is:

| Function | GPIO Pin |
|----------|----------|
| SPI0 MOSI (DIN) | GP19 |
| SPI0 SCK (CLK) | GP18 |
| Display CS | GP17 |
| Display DC | GP16 |
| Display RST | GP20 |
| Display BL | GP21 |
| Button A | GP12 |
| Button B | GP13 |
| Button X | GP14 |
| Button Y | GP15 |

> **Note**: You can modify these pins in `src/main.cpp` to match your hardware setup.

## Software Requirements

### Prerequisites

1. **Pico SDK**: Install the Raspberry Pi Pico SDK
   ```bash
   # Clone the SDK
   git clone https://github.com/raspberrypi/pico-sdk.git
   cd pico-sdk
   git submodule update --init
   
   # Set environment variable
   export PICO_SDK_PATH=/path/to/pico-sdk
   ```

2. **CMake**: Version 3.13 or higher
   ```bash
   sudo apt install cmake
   ```

3. **ARM GCC Toolchain**:
   ```bash
   sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
   ```

4. **Build Tools**:
   ```bash
   sudo apt install build-essential
   ```

## Project Structure

```
pico-test/
├── CMakeLists.txt              # Main build configuration
├── lib/
│   ├── lvgl/                   # LVGL graphics library
│   │   ├── lv_conf.h          # LVGL configuration
│   │   └── src/               # LVGL source files
│   └── st7789/                # ST7789 display driver
│       ├── st7789.c
│       └── include/
├── src/
│   ├── main.cpp               # Main application
│   ├── st7789_lvgl_driver.cpp # LVGL display driver adapter
│   ├── st7789_lvgl_driver.h
│   ├── debounced_button.cpp   # Button handling
│   └── debounced_button.h
└── build/                     # Build output directory
```

## Building the Project

### 1. Clone the Repository

```bash
git clone git@github.com:mad201802/pico_lvgl.git
cd pico_lvgl
```

### 2. Clone LVGL Library

The LVGL library is not included in version control. Clone it into the `lib/` directory:

```bash
# Clone LVGL into lib/lvgl
git clone https://github.com/lvgl/lvgl.git lib/lvgl

# Checkout a stable version (optional but recommended)
cd lib/lvgl
git checkout release/v9.2
cd ../..
```

> **Note**: Make sure you have the `lv_conf.h` file in the `lib/lvgl/` directory. This project includes a pre-configured version optimized for Raspberry Pi Pico.

### 3. Set Up Environment

Make sure the Pico SDK path is set:

```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

You can add this to your `~/.bashrc` or `~/.zshrc` to make it permanent.

### 4. Build

```bash
# Create and enter build directory
mkdir -p build
cd build

# Configure the project
cmake ..

# Build the project
make -j$(nproc)
```

The build process will generate several output files:
- `my_project.uf2` - Firmware file for drag-and-drop programming
- `my_project.elf` - ELF binary for debugging
- `my_project.bin` - Raw binary file
- `my_project.hex` - Intel HEX format

## Flashing the Pico

### Method 1: Drag-and-Drop (Easiest)

1. Hold the BOOTSEL button on your Pico
2. Connect the Pico to your computer via USB
3. Release the BOOTSEL button
4. The Pico will appear as a USB mass storage device
5. Copy `build/my_project.uf2` to the Pico drive
6. The Pico will automatically reboot and run your program

### Method 2: Using picotool

```bash
# Flash the UF2 file
picotool load build/my_project.uf2 -f

# Reboot the Pico
picotool reboot
```

## Using the Application

Once flashed, the application will:

1. Initialize the ST7789 display
2. Set up LVGL graphics library
3. Display a simple UI with:
   - Title label
   - Four button status indicators
   - Counter display
4. Respond to button presses (if buttons are connected)

### Serial Debug Output

Connect to the Pico's USB serial port to see debug messages:

```bash
# On Linux
sudo screen /dev/ttyACM0 115200

# Or using minicom
sudo minicom -D /dev/ttyACM0 -b 115200
```

## Configuration

### Display Settings

Edit `src/main.cpp` to change display configuration:

```cpp
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240
```

### LVGL Configuration

Edit `lib/lvgl/lv_conf.h` to customize LVGL behavior:

- Color depth (default: 16-bit RGB565)
- Memory settings
- Feature enable/disable
- Performance tuning

### Changing Main File

The project includes multiple example main files. Edit `CMakeLists.txt` to switch between them:

```cmake
add_executable(my_project
    src/main.cpp              # Original demo (default)
    # src/main_simple_test.cpp  # Simple display test
    # src/main_lvgl.cpp         # Full LVGL demo
    src/st7789_lvgl_driver.cpp
    src/debounced_button.cpp
)
```

## Troubleshooting

### Display Issues

- **No display output**: Check SPI pin connections
- **Garbled display**: Verify `DISPLAY_WIDTH` and `DISPLAY_HEIGHT` match your LCD
- **Wrong colors**: Adjust color format in `lv_conf.h`

### Build Errors

- **"PICO_SDK_PATH not found"**: Set the environment variable correctly
- **CMake errors**: Ensure CMake version is 3.13+
- **Linker errors**: Check that all required libraries are linked in `CMakeLists.txt`

### Memory Issues

The RP2040 has limited RAM (264KB). If you encounter memory issues:

1. Reduce LVGL buffer size in `st7789_lvgl_driver.cpp`
2. Disable unused LVGL features in `lv_conf.h`
3. Use simpler UI elements

## Resources

- [LVGL Documentation](https://docs.lvgl.io/)
- [Pico SDK Documentation](https://www.raspberrypi.com/documentation/pico-sdk/)
- [ST7789 Datasheet](https://www.waveshare.com/w/upload/a/ae/ST7789_Datasheet.pdf)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

## License

This project uses the following open-source components:
- **LVGL**: MIT License
- **Pico SDK**: BSD 3-Clause License

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Author

Created by mad201802

## Acknowledgments

- LVGL team for the excellent graphics library
- Raspberry Pi Foundation for the Pico SDK
- ST7789 driver contributors
