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
    uint64_t block_number;
    uint32_t record_offset_within_block;

    IndexEntry() : dni(0), block_number(0), record_offset_within_block(0) {}
    IndexEntry(uint32_t dni, uint64_t block_number, uint32_t record_offset_within_block)
        : dni(dni), block_number(block_number), record_offset_within_block(record_offset_within_block) {}

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & dni;
        ar & block_number;
        ar & record_offset_within_block;
    }
};

class DataManager {
public:
    DataManager(const std::string& data_filename, const std::string& index_filename, uint32_t records_per_block);
    ~DataManager();

    //bool file_existe(const string& filename);
    void writePerson(const Person& person, uint64_t& block_number, uint32_t& record_offset_within_block);
    bool readPerson(uint64_t block_number, uint32_t record_offset_within_block, Person& person);
    void updatePerson(uint64_t block_number, uint32_t record_offset_within_block, const Person& person);
    uint64_t getTotalBlocks() const;
    bool loadBlock(uint64_t block_number, std::vector<Person>& records);
private:
    fstream data_file;
    string data_filename;
    uint32_t records_per_block;
    uint64_t block_number;
    vector<Person> block_records;
    fstream index_file;
    string index_filename;
    uint64_t total_blocks;
    
    
    
    void compressAndWriteBlock();
    uint64_t getBlockOffset(uint64_t block_number);
    void loadBlockIndex();
    void updateBlockIndex(uint64_t block_number, uint64_t block_offset);
};

#endif // DATAMANAGER_H
