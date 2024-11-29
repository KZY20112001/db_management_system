# SC3020 Database System Principles

## Overview 
The project aims to implement two components of a database system: storage and indexing. For indexing, a B+ tree is used. Refer to Project Description for more details.  

## Installation Guide
1) Install Compiler
   - Install MinGW-x64 from [MSYS2](https://www.msys2.org/)
   - Add the path of your MinGW-w64 bin folder to the Windows PATH environment variable
   - Ensure you have C++ version 17
2) Edit filePath in main.cpp to your path for games.txt
3) Set directory to project directory
4) Run following command to compile code:
```bash
g++ -std=c++17 main.cpp b-plus-tree.cpp disk_storage.cpp -o main.exe
```
5) Run the main.exe file.
