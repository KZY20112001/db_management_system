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

void Block::listRecord() 
{
    if (numrecords == 0) {
        cout << "No records available." <<endl;
        return;
    }

    cout << "Listing records in the block:" <<endl;
    for (int n=0; n < numrecords ; n++) {
        Record rec = *reinterpret_cast<Record*>(reservedspace + sizeof(int) * 3 + sizeof(unsigned char*) + (n * sizeof(Record)));
        cout << "Record ID: " << rec.FG_PCT_home << ", TEAM_ID_home: " 
        << rec.TEAM_ID_home << ", PTS_Home: " 
        << rec.PTS_home << ", FT_PCT_home: " 
        << rec.FT_PCT_home << ", FG3_PCT_home: " 
        << rec.FG3_PCT_home << ", AST_home: " 
        << rec.AST_home << ", REB_home: " 
        << rec.REB_home <<endl;
    }
}

Block::~Block() 
{
    if (reservedspace != nullptr) {
        delete[] reservedspace;
        reservedspace = nullptr;
    }
}

Disk_Storage::Disk_Storage(int recordsize, int maxblocks, int blocksize):
    maxblocks(maxblocks), blocksize(blocksize), blocksused(0), availblocks(maxblocks), numrecords(0), memoryused(0), recordsize(recordsize)
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
        throw runtime_error("Unable to reserve space for record");
    }
    if (blockptr == nullptr || blockptr->availsize < recordsize){
        if(!this->addBlock()){
            throw runtime_error("Unable to reserve space for record");
        }
    }
    // create a function to add record to block and return record
    int offset = blocksize - blockptr->availsize;
    unsigned char* recordAddress = blockptr->reservedspace + offset;
    Record_Location header = {blocksused, offset};

    memcpy(recordAddress, &record, recordsize);
    blockptr->availsize -= recordsize;
    blockptr->numrecords += 1;
    this->numrecords += 1;
    this->memoryused += recordsize;

    return make_tuple(header, record.FG_PCT_home);
}

void Disk_Storage::listBlocks() {
    for (const auto& pair : blockmap) {
        cout << "Block Index: " << pair.first << ", Block Pointer: " << pair.second <<",  Number of records: " << pair.second->numrecords <<", Remaining space: " << pair.second->availsize <<endl;
    }
}

void Disk_Storage::listSpecificBlock(int id) {
    if (id > blocksused) throw runtime_error("Block not found");
    auto it = blockmap.find(id);
    Block* block = it->second;
    cout << "Block Index: " << id << ", Block Pointer: " << block <<",  Number of records: " << block->numrecords <<", Remaining space: " << block->availsize <<endl;
    block->listRecord();
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
