#ifndef DISK_STORAGE_H
#define DISK_STORAGE_H

#include <string>
#include <fstream>
#include <tuple>
#include <vector>
#include <unordered_map>
#include "Record.h"

const int BLOCK_SIZE = 4096;
const int MAX_BLOCK_ALLOWED = 1000;

struct Record_Location
{
    int blocknum;
    int offset;
};


class Block
{
    public:
        int availsize;
        int numrecords;
        unsigned char reservedspace[4096 - sizeof(int) * 2 - sizeof(unsigned char*)];
        Block();
        void listRecord();
        virtual ~Block();
};

class Disk_Storage
{
    public:
        unsigned char* storageptr;
        Block* blockptr;
        
        std::unordered_map<int, Block*> blockmap;
        int maxblocks;
        int blocksize;
        int blocksused;
        int availblocks;
        int numrecords;
        int memoryused;
        int recordsize;
        Disk_Storage(int recordsize, int maxblock = MAX_BLOCK_ALLOWED, int blocksize = BLOCK_SIZE);
        std::tuple<Record_Location, float> writeRecord(int recordsize, Record record);
        Record retrieveRecord(Record_Location recordlocation);
        std::tuple<int, float> linearScan(float start, float end);
        void listBlocks();
        void listSpecificBlock(int id);
        virtual ~Disk_Storage();

    private:
        bool addBlock();
};

#endif // DISK_STORAGE_H
