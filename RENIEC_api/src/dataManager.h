#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <vector>
#include <fstream>
#include "persona.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

using namespace std;

struct IndexEntry {
    uint32_t dni;
    uint32_t block_number;
    uint32_t record_offset_within_block;

    IndexEntry() : dni(0), block_number(0), record_offset_within_block(0) {}
    IndexEntry(uint32_t dni, uint32_t block_number, uint32_t record_offset)
        : dni(dni), block_number(block_number), record_offset_within_block(record_offset) {}
};

class DataManager {
public:
    DataManager(const std::string& data_filename, const std::string& index_filename, uint32_t records_per_block);
    ~DataManager();

    
    void writePerson(const Person& person, uint32_t& block_number, uint32_t& record_offset_within_block);
    bool readPerson(uint32_t block_number, uint32_t record_offset_within_block, Person& person);
    void updatePerson(uint32_t block_number, uint32_t record_offset_within_block, const Person& person);
    uint32_t getTotalBlocks() const;
    bool loadBlock(uint32_t block_number, std::vector<Person>& records);
private:
    fstream data_file;
    string data_filename;
    uint32_t records_per_block;
    uint32_t block_number;
    vector<Person> block_records;
    fstream index_file;
    string index_filename;
    uint32_t total_blocks;
    
    
    
    void compressAndWriteBlock();
    uint32_t getBlockOffset(uint32_t block_number);
    void loadBlockIndex();
    void updateBlockIndex(uint32_t block_number, uint32_t block_offset);
};

#endif // DATAMANAGER_H
