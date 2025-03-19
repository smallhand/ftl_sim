# Introduction
Simulate a FTL (Flash Translation Layer) which supports user to read/write data from/to NAND Flash in sector unit, and convert the unit to page that is the unit of NAND Flash

User can fully writes data even overwrite multiple times because FTL will performs GC (Garbage Collection) under a threshold to erase invalid data and move valid data to an empty block.

This project focus on unit conversion and table mapping, and manage the data within 1 die. You need to have Nand Flash domain knowledge first.

The simulation of the NAND Flash space and available space for users as below

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



