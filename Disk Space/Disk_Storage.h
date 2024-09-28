#ifndef DISK_STORAGE_H
#define DISK_STORAGE_H

#include <string>
#include <fstream>

const int BLOCK_SIZE = 4096;
const int MAX_BLOCK_ALLOWED = 1000;

class Record
{
    public:

        Record();
        virtual ~Record();

    protected:

    private:

};

class Block
{
    public:
        unsigned char* reservedspace;
        unsigned char* endptr; 

        int blocksize;
        int availsize;
        int numrecords;
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
        tuple<void * , Record> writeRecord(int recordsize);
        virtual ~Disk_Storage();

    protected:

    private:
        std::fstream file;
        std::string filename;
};

#endif // DISK_STORAGE_H
