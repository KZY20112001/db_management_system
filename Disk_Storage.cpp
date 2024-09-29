#include "Disk_Storage.h"
#include <iostream>
#include <cstring>

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
    if (reservedspace != nullptr) {
        delete[] reservedspace;
        reservedspace = nullptr;
    }
}

Disk_Storage::Disk_Storage(int recordsize, int maxblocks, int blocksize):
    recordsize(recordsize), maxblocks(maxblocks), blocksize(blocksize), availblocks(maxblocks), blocksused(0), numrecords(0), memoryused(0)
{
    storageptr = new unsigned char[maxblocks * blocksize];
    blockptr = nullptr;
}

bool Disk_Storage::addBlock()
{
        if (availblocks >= 0) {
            unsigned char* blocklocation = storageptr + (blocksused * blocksize);
            blockptr =  new (blocklocation) Block();
            availblocks -= 1;
            blocksused += 1;

            blockmap[blocksused] = blockptr;
            return true;
        }
        else {
            cout << "No available blocks left";
            return false;
        }
}

tuple<Record_Location, float> Disk_Storage::writeRecord(int recordsize, Record record)
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
    int offset = blocksize - blockptr->availsize;
    unsigned char* recordAddress = blockptr->reservedspace + offset;
    Record_Location header = {blocksused, offset};

    memcpy(recordAddress, &record, recordsize);
    blockptr->availsize -= recordsize;
    blockptr->numrecords += 1;
    this->memoryused += recordsize;

    return make_tuple(header, record.FG_PCT_home);
}

Disk_Storage::~Disk_Storage()
{
    if (storageptr != nullptr) {
            for (int i = 0; i < blocksused; i++) {
                Block* block = reinterpret_cast<Block*>(storageptr + (i * blocksize));
                block->~Block();
            }

            delete[] storageptr;
            storageptr = nullptr;
    }
}
