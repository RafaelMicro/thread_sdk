Before you start building the examples, you must download and install the toolchain and the tools required for flashing and debugging.

## Toolchain

In a Bash terminal, follow these instructions to install the GNU toolchain and other dependencies.

```bash
$ cd <path-to-thread-sdk>
$ ./script/bootstrap
```

## Building

In a Bash terminal, follow these instructions to build the rt58x examples.

```bash
$ cd <path-to-thread-sdk>
$ ./script/build rt58x -DOT_THREAD_VERSION=1.3
```
Build a sleepy device 
```bash
$ cd <path-to-thread-sdk>
$ ./script/build rt58x -DOT_THREAD_VERSION=1.3 -DPLAFFORM_CONFIG_ENABLE_SLEEP=1
```

## Flash Binaries

To load the images with the isp download tool, the images must be converted to `bin`. This is done using `arm-none-eabi-objcopy`

```bash
$ cd <path-to-thread-sdk>
$ arm-none-eabi-objcopy -O binary build/bin/ot-rt58x-ftd build/bin/ot-rt58x-ftd.bin
$ arm-none-eabi-objcopy -O binary build/bin/ot-rt58x-mtd build/bin/ot-rt58x-mtd.bin
```

