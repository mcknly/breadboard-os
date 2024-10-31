# [ B r e a d b o a r d O S ]
```
                             ___          ___
    _____       _____       /  /\        /  /\
   /  /::\     /  /::\     /  /::\      /  /:/_
  /  /:/\:\   /  /:/\:\   /  /:/\:\    /  /:/ /\
 /  /:/~/::\ /  /:/~/::\ /  /:/  \:\  /  /:/ /::\
/__/:/ /:/\:/__/:/ /:/\:/__/:/ \__\:\/__/:/ /:/\:\
\  \:\/:/~/:|  \:\/:/~/:|  \:\ /  /:/\  \:\/:/~/:/
 \  \::/ /:/ \  \::/ /:/ \  \:\  /:/  \  \::/ /:/
  \  \:\/:/   \  \:\/:/   \  \:\/:/    \__\/ /:/
   \  \::/     \  \::/     \  \::/       /__/:/
    \__\/       \__\/       \__\/        \__\/

```
  
**BreadboardOS** (BBOS, if you like) - as its namesake implies - is a firmware platform aimed at quick prototyping. BBOS was built around the following principles:
1. **_Every project should start with a CLI._** A command line interface makes prototyping, debugging, and testing about 1000% easier, especially over the lifespan of multiple projects with re-used pieces.
2. **_Write it once._** Don't waste time writing and re-writing the basic infrastructure for your projects. Modularize, duplicate, and profit!
3. **_Get running QUICKLY._** The faster you can bring up a project, the faster you can find out that you need to re-tool the whole dang thing. _But at least you don't have to write a CLI again!_

## Functional Description

BreadboardOS is built on top of FreeRTOS, enabling fast integration of new functional blocks and allowing for task concurrency without breaking the base system. Existing knowledge of RTOS is not necessarily required.  
  
The central component of BBOS is the fantastic [microshell](https://microshell.pl/) project, which provides CLI functionality. Currently, a [fork](https://github.com/mcknly/microshell) of microshell is used, which includes some additional customization. The CLI implementation is organized into POSIX-style folders/files providing a recognizable user-interface for interacting with MCU hardware.  
  
As of the v0.3 release, BBOS is implemented on a single MCU family - the Raspberry Pi RP2xxx (Pico, Pico W, Pico2, etc). However, the project has been structured such that all hardware-specific code resides in a single directory, with a header file providing HAL functionality. The platform was built with porting in mind.

### _Notable Features_

- Rich set of informational tools for checking system resources on the fly, a'la: `ps top free df`
- RTOS task manager for dynamically controlling system services at runtime
- Onboard flash filesystem with wear-leveling (thanks to [littlefs](https://github.com/littlefs-project/littlefs))
- Selective peripheral hardware initialization routines
- Interacting with chip I/O and serial buses directly from command line
- Watchdog service for system failsafe recovery
- Command history using arrows or ctrl-a/ctrl-z

### _Demo_

As they say, a Youtube video is worth 10^6 words.  
[https://www.youtube.com/watch?v=fQiYE_wlPt0](https://www.youtube.com/watch?v=fQiYE_wlPt0)

## Getting started

The following build instructions for BreadboardOS assume a GNU/Linux environment, or alternatively WSL under Windows. Building in other environments may be possible but is not covered here.

### Prerequisites

- A basic embedded ARM development enviroment: `build-essential gcc-arm-none-eabi newlib-arm-none-eabi cmake`
- The [FreeRTOS Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel) in an accessible location on the system
- The Raspberry Pi [Pico SDK](https://github.com/raspberrypi/pico-sdk) in an accessible location on the system

Ensure that the environment variables `$FREERTOS_KERNEL_PATH` and `$PICO_SDK_PATH` are set and contain the absolute paths to those projects:  
  
`export FREERTOS_KERNEL_PATH=/path/to/freertos/kernel`  
`export PICO_SDK_PATH=/path/to/pico/sdk`  
  
It is suggested to add these commands to the user's `~/.profile`, `~/.bashrc`, etc depending on the system.

**NOTE for RP2350**: As of 10/2024, official FreeRTOS has not yet merged in support for RP2350. Rasberry Pi's [fork](https://github.com/raspberrypi/FreeRTOS-Kernel) must be used. Further, there is a [bug](https://forums.raspberrypi.com/viewtopic.php?p=2253073#p2253073) in the Pico SDK, use the `develop` branch to incorporate the fix. Hopefully these changes will be merged upstream soon.

### Setting up BreadboardOS

- Clone the repository: `git clone https://github.com/mcknly/breadboard-os.git`
- Enter the project folder: `cd breadboard-os`
- Pull in submodules: `git submodule update --init`

Open `project.cmake` and edit the following parameters:
```
# PROJECT NAME - in quotes, no spaces
set(PROJ_NAME "my-bbos-proj")

# PROJECT VERSION - in quotes, no spaces, can contain alphanumeric if necessary
set(PROJ_VER "0.0")

# CLI INTERFACE - 0: use UART for CLI (default), 1: use USB for CLI
set(CLI_IFACE 0)

# MCU PLATFORM - set the MCU platform being used (i.e. the subdir in 'hardware/')
set(PLATFORM rp2xxx)

# BOARD - set the board being used (platform-specific prebuild.cmake contains more information about boards)
set(BOARD pico2)
```

Using CLI over USB requires no additional hardware; using CLI over UART will require a USB-UART adapter (i.e. FTDI FT232 or SiLabs CP2102). Using CLI/UART enables some additional early boot status prints.

### Building

- Create & enter build folder: `mkdir build && cd build`
- Generate the build files with cmake: `cmake ..` (note: this will default to a 'Release' build)
- Compile the project: `make -j4` (# cores can be altered)

This should generate firmware image formats `.bin`, `.hex`, `.uf2`, to be programmed to flash via your preferred method.

### Debugging

For a full debugging-enabled IDE-like experience (recommended), using MS VS Code plus the `C/C++`, `CMake`, and `Cortex-Debug` extensions with your debug probe of choice (Pi Debug Probe, J-Link, etc) is the widely preferred method. See [this](https://www.digikey.com/en/maker/projects/raspberry-pi-pico-and-rp2040-cc-part-2-debugging-with-vs-code/470abc7efb07432b82c95f6f67f184c0) great tutorial by DigiKey.

## Usage - building your own application

Creating your own application on the BreadboardOS platform entails adding one or more new "services" - which are essentially standalone FreeRTOS tasks. An example of this is given in the `services/heartbeat_service.c` file, the structure of which can be used as a basis for your new service/application. See `services/services.h` which includes extensive documentation on how services/tasks are implemented on BBOS.  
  
To understand how your new service can interact with the system, refer to the function implementations in the file nodes within the `cli` directory - these have been provided as a test bed for the underlying system functionality, and the intention is for any new services/tasks to also have their functions mapped into new CLI folders & files - think of this as a test menu system for your project. The CLI could also be leveraged as an automated production test interface!

## Code Organization & Technical Detail

Project directory structure:  
- `cli/` - directory/file structure for the CLI, and interface to the microshell submodule
- `driver_lib/` - drivers for external peripheral hardware - library to grow over time
- `hardware/` - all hardware-specific (HAL) functional implementation, each MCU platform has its own subdirectory
- `littlefs/` - contains the 'littlefs' submodule and CMake wrapper
- `rtos/` - FreeRTOS interface & configuration files
- `services/` - where the main application elements (FreeRTOS tasks) reside
- `git_version/` - submodule for adding GIT repo metadata into the binary
- `microshell/` - submodule for microshell

## Contributing

Contributions to the project are welcome! Here are some specific ways to contribute:
- Use the project! build cool things, identify bugs, submit issues
- Add additional peripheral hardware drivers to `driver_lib`
- Create useful utility services
- Port the project to a new MCU platform

To contribute - fork the project, make enhancements, submit a PR.

## Acknowledgements

BreadboardOS is built in no small part upon the substantial work of others:  
[microshell](https://github.com/marcinbor85/microshell)  
[littlefs](https://github.com/littlefs-project/littlefs)  
[cmake-git-version-tracking](https://github.com/andrew-hardin/cmake-git-version-tracking)  
[freertos](https://github.com/FreeRTOS/FreeRTOS-Kernel)  
[pico-sdk](https://github.com/raspberrypi/pico-sdk)  

## License

This project is released under the [MIT License](https://opensource.org/license/mit/).  
In lieu of including the full license text in every source file, the following tag is used:  
`// SPDX-License-Identifier: MIT`  
  
---
---
  
<div align="center">
<img src=breadboard.png alt="breadboard" width="600" align=center/>
<br>
it's fresh
</div>
