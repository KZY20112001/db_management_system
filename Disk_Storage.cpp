#include "disk_storage.h"
#include <iostream>
#include <cstring>

using namespace std;

Block::Block():
    numrecords(0)
{
    int blockheadersize = sizeof(int) * 2 + sizeof(unsigned char*);
    availsize = 4096 - blockheadersize;
}

// Method to display all records in the block for debugging
void Block::listRecord() 
{
    if (numrecords == 0) {
        cout << "No records available." <<endl;
        return;
    }

    cout << "Listing records in the block:" <<endl;
    for (int n=0; n < numrecords ; n++) {
        Record rec = *reinterpret_cast<Record*>(reservedspace + (n * sizeof(Record)));
        cout << "TEAM_ID_home: 16106127" << rec.TEAM_ID_home << ", "
        << "PTS_home: " << static_cast<int>(rec.PTS_home) << ", "
        << "FG_PCT_home: " << rec.FG_PCT_home << ", "
        << "FT_PCT_home: " << rec.FT_PCT_home << ", "
        << "FG3_PCT_home: " << rec.FG3_PCT_home << ", "
        << "AST_home: " << static_cast<int>(rec.AST_home) << ", "
        << "REB_home: " << static_cast<int>(rec.REB_home) << ", "
        << "HOME_TEAM_WINS: " << rec.HOME_TEAM_WINS
        << endl;
    }
}


Block::~Block() 
{
}

Disk_Storage::Disk_Storage(int recordsize, int maxblocks, int blocksize):
    maxblocks(maxblocks), blocksize(blocksize), blocksused(0), availblocks(maxblocks), numrecords(0), memoryused(0), recordsize(recordsize)
{
    storageptr = new unsigned char[maxblocks * blocksize];
    blockptr = nullptr;
}

bool Disk_Storage::addBlock()
{
    //check if theres enough space for more blocks
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
    // calc offset from block start to space available for record
    int offset = blocksize - blockptr->availsize;
    unsigned char* recordAddress = reinterpret_cast<unsigned char*>(blockptr) + offset;
    // create key for finding record for b+ tree
    Record_Location header = {blocksused, offset};

    // write record to block
    memcpy(recordAddress, &record, recordsize);
    blockptr->availsize -= recordsize;
    blockptr->numrecords += 1;
    this->numrecords += 1;
    this->memoryused += recordsize;

    // return key to record
    return make_tuple(header, record.FG_PCT_home);
}

// Method to display all blocks in the disk for debugging
void Disk_Storage::listBlocks() {
    for (const auto& pair : blockmap) {
        cout << "Block Index: " << pair.first << ", Block Pointer: " << pair.second <<",  Number of records: " << pair.second->numrecords <<", Remaining space: " << pair.second->availsize <<endl;
    }
}

// Method to display selected block in the disk for debugging
void Disk_Storage::listSpecificBlock(int id) {
    if (id > blocksused) throw runtime_error("Block not found");
    auto it = blockmap.find(id);
    Block* block = it->second;
    cout << "Block Index: " << id << ", Block Pointer: " << block <<",  Number of records: " << block->numrecords <<", Remaining space: " << block->availsize <<endl;
    block->listRecord();
}

// Method to retrieve record using the key
Record Disk_Storage::retrieveRecord(Record_Location recordlocation) {
    if (recordlocation.blocknum > blocksused || recordlocation.blocknum <= 0) throw runtime_error("Invalid block number");
    if (recordlocation.offset > blocksize || recordlocation.offset < static_cast<int>(sizeof(int) * 2 + sizeof(unsigned char*))) throw runtime_error("Invalid offset value");

    auto it = blockmap.find(recordlocation.blocknum);
    Block* block = it->second;
    Record rec = *reinterpret_cast<Record*>(reinterpret_cast<unsigned char*>(block) + recordlocation.offset);
    return rec;
}

tuple<int, float> Disk_Storage::linearScan(float start, float end) {
    int iocounter = 0;
    int recordcounter = 0;
    float sumFG3_PCT_home = 0.0;

    for (int n = 1; n <= blocksused; n++) {
        Block* blkptr = blockmap.find(n)->second;
        iocounter++;
        for (int r = 0; r < blkptr->numrecords; r++) {
            Record rec = *reinterpret_cast<Record*>(blkptr->reservedspace + (r * sizeof(Record)));
            if (start <= rec.FG_PCT_home && rec.FG_PCT_home <= end) {
                ++recordcounter;
                sumFG3_PCT_home += rec.FG3_PCT_home; // Accumulate FG3_PCT_home
            }
        }
    }

    float averageFG3_PCT_home = recordcounter > 0 ? sumFG3_PCT_home / recordcounter : 0.0f;
    // cout << "Total Records: " << recordcounter << endl;
    return make_tuple(iocounter, averageFG3_PCT_home); // Return IO count and average
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
