# tp2-so-2015b ~~tp-arqui-q1-2015~~

![Travis CI Build Status](https://travis-ci.org/pcostesi/tp2-so-2015b.svg?branch=master)

From the ashes of The Barely Compiles Operating System comes... ATLAS/CMS ®©™ Mainframe Corporate Edition.

[TOC]

## Building

## TL;DR
```bash
wget http://newos.org/toolchains/x86_64-elf-4.9.1-Linux-x86_64.tar.xz
tar xvf x86_64-elf-4.9.1-Linux-x86_64.tar.xz
mv "x86_64-elf-4.9.1-Linux-x86_64" "${HOME}/x86_64-elf-4.9.1-Linux-x86_64"
cp Toolchain.inc.sample Toolchain.inc
make
```

## Using a cross-compiler
At the root of the project add a `Toolchain.inc` file with the following variables pointing to your cross compiler:

- GCC: gcc cross compiler
- GPP: g++. Not needed, but...
- LD: Linker.
- AR: Archiver. Not used.
- ASM: nasm. Try not to change this.

You can download a ready-made cross-compiler from the [OSDev site](http://wiki.osdev.org/GCC_Cross-Compiler). We recommend the __[x86_64-elf 4.9.1 target](http://newos.org/toolchains/x86_64-elf-4.9.1-Linux-x86_64.tar.xz)__

### Ubuntu
In order to build the system, first install the dependencies with `make deps`. This will install `gcc`, `nasm` and `make` (which you should already have, or you wouldn't be able to run `make`).

The system has several stages:
- Toolchain: build tools such as packer.
- Bootloader: based on the x86barebones project, will build the base system image
- Kernel: build just the kernel object files
- Userland: build kernel and toolchain along with the userland code.
- Image: pack the kernel, userland and bootloader.

## Running

Just do `make run`. If you need to run raw images, use `make raw`

Running the OS requires qemu. This is not possible in Travis because it uses an old version of Ubuntu.

