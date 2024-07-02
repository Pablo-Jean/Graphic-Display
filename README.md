# 📊 Graphic Display

##  🔍 About the Library

Graphic Display is a C library designed for controlling monochromatic displays without any chipset or microcontroller dependency. This library allows you to use any display by providing a driver logic, making it a versatile solution for your project.


## ✨ Features

*   **Chipset and Microcontroller Independent**: Graphic Display does not rely on any specific chipset or microcontroller, allowing you to use it with a wide range of hardware.
*   **Customizable Driver Logic**: The library provides a flexible interface for implementing your own display driver logic, enabling you to support various types of displays.
*   **Cross-platform Compatibility**: Graphic Display is designed to be platform-independent, making it easy to integrate into your projects across different operating systems.
*   **Efficient and Lightweight**: The library is optimized for performance and has a small footprint, ensuring minimal impact on your project's resources.
  
## 🗂️ API Reference
    
Access the Link [Graphic Display API Reference](https://pablo-jean.github.io/Graphic-Display) to see detailed information about functions, structs and more.

## 🛠️ Installation

To use Graphic Display in your project, follow these steps:

1.  git clone `git@github.com:Pablo-Jean/Graphic-Display.git`.
2.  Include the library into source code compilation and include paths.
3.  ```#include "GraphicDisplay.h"```.
4.  Link the library to your project during compilation.

Do not forget to remove examples from source code path :).
    
## 🚀 Usage

Here's a basic example of how to use the Graphic Display library with **SSD1306** display in I2C mode:

```C
#include "GraphicDisplay.h"
#include "drivers/ssd1306/ssd1306.h"

/* Function to handle the I2C interface 
   This routine will be provided do SSD1306 library instance. */
void _display_i2c_interface(uint8_t i2cAddr, uint8_t reg, uint8_t *buff, uint32_t len){
    /* Call the I2C driver of your microcontroller */
    uint8_t resp = I2C_Write_mem(i2cHandler, i2cAddr, reg, buff, len);

    return resp;
}

void main (){
    // Creates SSD1306 instance. We recommend to initialize as 0.
	ssd1306_t Ssd1306 = {0};
    // Creates struct with parameters of SSD1306
    // This structure is different according with display interface chip.
	ssd1306_params_t Ssd1306_Params = {
        // set I2C mode
        .Mode = SSD1306_MODE_I2C,
        // Set Heighth and Width (128x64)
        .u32Heigth = 64,
        .u32Width = 128,
        // We do not provide the Frame Buffer array, so, the library will allocate internally.
        .bUseExternalFrameBuffer = false,
        // provide the Delay routine
        .delayMs = DelayMs,
        // provide the routine to handle the I2C display interface
        .i2cWrite = _display_i2c_interface,
        // provide the SSD1306 address (remember, bit 7 must aways be 0)
        // check if your i2c library requires to shift 1 bit to left (example: STM32 HAL requires this)
        .u8I2CAddr = 0x3C
	};

    // Created Graphic Display library instance. We recommend you to initialize is as 0
	gd_t Graphic = {0};
    // Create struct with the Graphic Display parameters
	gd_params_t Graphic_Params = {
        // Provide the drivers to work with SSD1306, for libraries place into drivers folder
        // we have this already created, the naming of this Driver follows this logic:
        // Gd_Driver_XXXX
        // where XXXX is the name of device driver, always in UPPER CASE.
        //
        // This Driver is just a struct containing pointers to the functions of the display.
        .DisplayDriver = Gd_Driver_SSD1306,
        // Provide the handle instance of the display, as a pointer
        .DisplayHandle = (void*)(&Ssd1306),
        // Provide the Height and Width of your display
        .u32Height = 64,
        .u32Width = 128
	};

    // Initialize the display device
    SSD1306_Init(&Ssd1306, &Ssd1306_Params);

    // Initialize the Graphic Display layer
    GD_Init(&Graphic, &Graphic_Params);

    // Write your text, or symbols, or images

    // To see all changes on display, you need to refresh manually
    GD_UpdateScreen(&Graphic);
    // You do not need to call the device driver refresh, Graphic Display library to this for you.
}

```

## 💡 Examples

You can find various examples of using the Graphic Display library in the **examples** directory. These examples cover different use cases and demonstrate the library's capabilities.

## 🤝 Contributing

We welcome contributions to the Graphic Display library! If you find any issues or have ideas for improvements, please feel free to submit a pull request or open an issue on the [GitHub repository](https://github.com/pablo-jean/graphic-display).

## 🛠️ Creating Your Driver

You can write your own driver to use with this library, the only requirement is that your display is Monochromatic, this library doesn't support color displays.

Feel free to perform a Fork, implement your driver, and make a Pull request, sharing your contributing with other developers.

To see what you need to do, check the API reference.

## 📄 License

Graphic Display is released under the MIT License. Feel free to use, modify, and distribute the library as per the terms of the license.

## Credits

This library was based on [Afiskon/stm32-ssd1306](https://github.com/afiskon/stm32-ssd1306).
