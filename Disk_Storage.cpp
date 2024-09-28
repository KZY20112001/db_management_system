#include "Disk_Storage.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <tuple>

using namespace std;

Block::Block(int blocksize):
    blocksize(blocksize), numrecords(0)
{
    int blockheadersize = sizeof(int) * 3 + sizeof(unsigned char*);
    reservedspace = new unsigned char[blocksize - blockheadersize];
    availsize = blocksize - blockheadersize;
}

Block::~Block() 
{

}

Disk_Storage::Disk_Storage(int recordsize, int maxblocks, int blocksize):
    recordsize(recordsize), maxblocks(maxblocks), blocksize(blocksize), availblocks(maxblocks), blocksused(0), numrecords(0), memoryused(0)
{
    storageptr = nullptr;
    blockptr = nullptr;
}

bool Disk_Storage::addBlock()
{
        if (availblocks == 0) {
            storageptr = new Block();
            blockptr = storageptr;
            availblocks -= 1;
            blocksused += 1;
            return true;
        }
        else if (availblocks > 0) {
            blockptr = storageptr + (blocksused * blocksize);
            new (blockptr) Block();
            availblocks -= 1;
            blocksused += 1;
            return true;
        }
        else {
            cout << "No available blocks left";
            return false;
        }
}

tuple<void*, Record> Disk_Storage::writeRecord(int recordsize, Record record)
{
    if (recordsize > blocksize) {
        throw "Unable to reserve space for record";
    }
    if (blockptr == nullptr || blockptr->availsize < recordsize){
        if(!this->addBlock()){
            throw "Unable to reserve space for record";
        }
    }
    // create a function to add record to block and return record
    unsigned char* recordAddress = blockptr->reservedspace + (blocksize - blockptr->availsize);
    memcpy(recordAddress, &record, recordsize);
    blockptr->availsize -= recordsize;
    blockptr->numrecords += 1;
    this->memoryused += recordsize;
    return make_tuple(static_cast<void*>(recordAddress), record);
}

Disk_Storage::~Disk_Storage()
{

}
