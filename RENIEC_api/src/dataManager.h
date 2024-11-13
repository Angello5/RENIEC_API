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
    size_t block_number;
    size_t record_offset_within_block;

    IndexEntry() : dni(0), block_number(0), record_offset_within_block(0) {}
    IndexEntry(uint32_t dni, size_t block_number, size_t record_offset)
        : dni(dni), block_number(block_number), record_offset_within_block(record_offset) {}
};

class DataManager {
public:
    DataManager(const std::string& data_filename, const std::string& index_filename, size_t records_per_block);
    ~DataManager();

    void writePerson(const Person& person, size_t& block_number, size_t& record_offset_within_block);
    bool readPerson(size_t block_number, size_t record_offset_within_block, Person& person);
    void updatePerson(size_t block_number, size_t record_offset_within_block, const Person& person);

private:
    fstream data_file;
    string data_filename;
    size_t records_per_block;
    size_t block_number;
    vector<Person> block_records;
    fstream index_file;
    string index_filename;
    size_t total_blocks;
    
    
    
    void compressAndWriteBlock();
    size_t getBlockOffset(size_t block_number);
    void loadBlock(size_t block_number, std::vector<Person>& records);
    void loadBlockIndex();
    void updateBlockIndex(size_t block_number, size_t block_offset);
};

#endif // DATAMANAGER_H
