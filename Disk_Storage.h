#ifndef DISK_STORAGE_H
#define DISK_STORAGE_H

#include <string>
#include <fstream>
#include <tuple>
#include <vector>
#include "Record.h"

const int BLOCK_SIZE = 4096;
const int MAX_BLOCK_ALLOWED = 1000;

class Block
{
    public:
        int blocksize;
        int availsize;
        int numrecords;
        unsigned char* reservedspace;
        Block(int blocksize = BLOCK_SIZE);
        virtual ~Block();

    protected:

    private:
};

class Disk_Storage
{
    public:
        Block* storageptr;
        Block* blockptr;

        int maxblocks;
        int blocksize;
        int blocksused;
        int availblocks;
        int numrecords;
        int memoryused;
        int recordsize;
        Disk_Storage(int recordsize, int maxblock = MAX_BLOCK_ALLOWED, int blocksize = BLOCK_SIZE);
        bool addblock();
        std::tuple<void*, Record> writeRecord(int recordsize);
        virtual ~Disk_Storage();

    protected:

    private:
        std::fstream file;
        std::string filename;
};

#endif // DISK_STORAGE_H
