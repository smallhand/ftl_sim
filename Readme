# Introduction
Simulate an user read/write data to NAND Flash in sector unit, while the FTL (Flash Translation Layer) converts the units in NAND Flash read/write units: page.
FTL supports user space to be fully written. It performs GC (Garbage Collection) to enable multiple overwriting.

This project focus on unit conversion and table mapping, and manage the data within 1 die. You need to have Nand Flash domain knowledge first.

Assume total space in the NAND Flash, and available space for users as below

| Units    | Nand Flash | 
| :-------- | :-------: |
| Block number  |  1024    |
| Page number / block | 32   |
| Page size    | 4KB    |

| Units    | User | 
| :-------- | :-------: |
| Block number  |  1000    |
| Sector number / page | 8 |
| Sector size    | 512B   |

# Run the project
Before executing, you need to install `make`.
Issue commands as below,

* `make all`: compile and generate bin files in `bin/` folder, and execute according different OS
    * Linux/Mac: `./bin/main`
    * Windows: `./bin/main.exe`
* `make clean`: clean all execute files under `bin/` folder



