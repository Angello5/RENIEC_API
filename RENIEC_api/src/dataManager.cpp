#include "dataManager.h"
#include <sstream>

DataManager::DataManager(const std::string& data_filename, const std::string& index_filename, size_t records_per_block)
    : data_filename(data_filename), index_filename(index_filename), records_per_block(records_per_block), block_number(0), total_blocks(0) {
    data_file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!data_file) {
        // Si el archivo no existe, créalo
        data_file.clear(); // Limpiar flags de error
        data_file.open(data_filename, std::ios::out | std::ios::binary);
        data_file.close();
        data_file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    index_file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!index_file) {
        // Si el archivo no existe, créalo
        index_file.clear(); // Limpiar flags de error
        index_file.open(index_filename, std::ios::out | std::ios::binary);
        index_file.close();
        index_file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    loadBlockIndex();
}



DataManager::~DataManager() {
    if (!block_records.empty()) {
        compressAndWriteBlock();
    }
    data_file.close();
    index_file.close();
}


void DataManager::writePerson(const Person& person, size_t& out_block_number, size_t& out_record_offset_within_block) {
    block_records.push_back(person);
    out_block_number = block_number;
    out_record_offset_within_block = block_records.size() - 1;

    if (block_records.size() >= records_per_block) {
        compressAndWriteBlock();
        block_records.clear();
        block_number++;
    }
}

void DataManager::compressAndWriteBlock() {
    // Serializar los registros a un buffer
    std::stringstream ss;
    boost::archive::binary_oarchive oa(ss);
    oa << block_records;

    // Comprimir el buffer
    std::string uncompressed_data = ss.str();
    std::string compressed_data;
    {
        std::stringstream compressed_ss;
        boost::iostreams::filtering_stream<boost::iostreams::output> out;
        out.push(boost::iostreams::zlib_compressor());
        out.push(compressed_ss);
        out.write(uncompressed_data.data(), uncompressed_data.size());
        out.flush();
        compressed_data = compressed_ss.str();
    }

    data_file.seekp(0, std::ios::end);
    size_t block_offset = data_file.tellp();
    
    uint32_t data_size = static_cast<uint32_t>(compressed_data.size());
    data_file.write(reinterpret_cast<char*>(&data_size), sizeof(data_size));
    data_file.write(compressed_data.data(), data_size);
    
    updateBlockIndex(block_number, block_offset);
}

size_t DataManager::getBlockOffset(size_t block_number) {
    if (block_number >= total_blocks) {
        // Manejar el error: bloque no existente
        return 0;
    }

    BlockIndexEntry entry;
    index_file.seekg(block_number * sizeof(BlockIndexEntry));
    index_file.read(reinterpret_cast<char*>(&entry), sizeof(BlockIndexEntry));

    return entry.block_offset;
}

void DataManager::loadBlock(size_t block_number, std::vector<Person>& records) {
    size_t block_offset = getBlockOffset(block_number);
    data_file.seekg(block_offset);

    uint32_t data_size;
    data_file.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));

    std::string compressed_data(data_size, '\0');
    data_file.read(&compressed_data[0], data_size);

    // Descomprimir el bloque
    {
        std::stringstream compressed_ss(compressed_data);
        boost::iostreams::filtering_stream<boost::iostreams::input> in;
        in.push(boost::iostreams::zlib_decompressor());
        in.push(compressed_ss);
        boost::archive::binary_iarchive ia(in);
        ia >> records;
    }
}

bool DataManager::readPerson(size_t block_number, size_t record_offset_within_block, Person& person) {
    std::vector<Person> records;
    loadBlock(block_number, records);

    if (record_offset_within_block < records.size()) {
        person = records[record_offset_within_block];
        return true;
    } else {
        return false;
    }
}

void DataManager::updatePerson(size_t block_number, size_t record_offset_within_block, const Person& person) {
    std::vector<Person> records;
    loadBlock(block_number, records);

    if (record_offset_within_block < records.size()) {
        records[record_offset_within_block] = person;

        // Reescribir el bloque completo
        // Serializar los registros a un buffer
        std::stringstream ss;
        boost::archive::binary_oarchive oa(ss);
        oa << records;

        // Comprimir el buffer
        std::string uncompressed_data = ss.str();
        std::string compressed_data;
        {
            std::stringstream compressed_ss;
            boost::iostreams::filtering_stream<boost::iostreams::output> out;
            out.push(boost::iostreams::zlib_compressor());
            out.push(compressed_ss);
            out.write(uncompressed_data.data(), uncompressed_data.size());
            out.flush();
            compressed_data = compressed_ss.str();
        }

        // Escribir el bloque comprimido al archivo en su posición original
        size_t block_offset = getBlockOffset(block_number);
        data_file.seekp(block_offset);
        uint32_t data_size = static_cast<uint32_t>(compressed_data.size());
        data_file.write(reinterpret_cast<char*>(&data_size), sizeof(data_size));
        data_file.write(compressed_data.data(), data_size);
    } else {
        // Manejar el error: posición de registro inválida
    }
}

void DataManager::loadBlockIndex() {
    index_file.seekg(0, std::ios::end);
    size_t index_size = index_file.tellg();
    total_blocks = index_size / sizeof(BlockIndexEntry);
}

void DataManager::updateBlockIndex(size_t block_number, size_t block_offset) {
    BlockIndexEntry entry = {block_number, block_offset};
    index_file.seekp(block_number * sizeof(BlockIndexEntry));
    index_file.write(reinterpret_cast<const char*>(&entry), sizeof(BlockIndexEntry));
    total_blocks = std::max(total_blocks, block_number + 1);
}
