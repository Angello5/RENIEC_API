#include "dataManager.h"
#include <sstream>
#include <iostream>

using namespace std;

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
        if (!data_file.is_open()) {
                std::cerr << "Error: No se pudo abrir el archivo de datos " << data_filename << std::endl;
                throw std::runtime_error("No se pudo abrir el archivo de datos");
            }
        
    index_file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!index_file) {
        // Si el archivo no existe, créalo
        index_file.clear(); // Limpiar flags de error
        index_file.open(index_filename, std::ios::out | std::ios::binary);
        index_file.close();
        index_file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
    }
        
        if (!index_file.is_open()) {
                std::cerr << "Error: No se pudo abrir el archivo de índice " << index_filename << std::endl;
                throw std::runtime_error("No se pudo abrir el archivo de índice");
            }
        
    loadBlockIndex();
}



DataManager::~DataManager() {
    if (!block_records.empty()) {
        compressAndWriteBlock();
    }
    data_file.flush();
    data_file.close();
    
    index_file.flush();
    index_file.close();
}


void DataManager::writePerson(const Person& person, size_t& out_block_number, size_t& out_record_offset_within_block) {
    block_records.push_back(person);
    out_block_number = block_number;
    out_record_offset_within_block = block_records.size() - 1;

    //std::cout << "Agregado persona con DNI: " << person.dni << " a block_records. Block number: " << block_number << ", record offset: " << out_record_offset_within_block << std::endl;
    
    if (block_records.size() >= records_per_block) {
        compressAndWriteBlock();
        block_records.clear();
        //block_number++;
    }
}

void DataManager::compressAndWriteBlock() {
    // Serializar los registros a un buffer
    std::stringstream ss;
    boost::archive::binary_oarchive oa(ss);
    oa << block_records;

    string serialized_data = ss.str();
    /*
    // Comprimir el buffer(vamos a probar si funciona sin comprimir y descomprimir)
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
    */
    data_file.seekp(0, std::ios::end);
    if (data_file.fail()) {
            std::cerr << "Error al hacer seekp al final del archivo de datos." << std::endl;
            data_file.clear();
            return;
        }
    
    size_t block_offset = data_file.tellp();
    
    //cambiar de compresed_data a serialized_data
    uint32_t data_size = static_cast<uint32_t>(serialized_data.size());
    data_file.write(reinterpret_cast<char*>(&data_size), sizeof(data_size));
    if (data_file.fail()) {
            std::cerr << "Error al escribir el tamaño de los datos en el archivo de datos en el bloque " << block_number << std::endl;
            data_file.clear();
            return;
        }
    data_file.write(serialized_data.data(), data_size);
    if (data_file.fail()) {
           std::cerr << "Error al escribir los datos serializados en el archivo de datos en el bloque " << block_number << std::endl;
           data_file.clear();
           return;
       }
    data_file.flush();
        if (data_file.fail()) {
            std::cerr << "Error al hacer flush del archivo de datos después de escribir el bloque " << block_number << std::endl;
            data_file.clear();
            return;
        }
    
    updateBlockIndex(block_number, block_offset);
    
    block_number++;
    
    //cout<<"Bloque escrito. block_number: "<< block_number << ", total_blocks" <<total_blocks<<endl;
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

size_t DataManager::getTotalBlocks() const {
    return total_blocks;
}

bool DataManager::loadBlock(size_t block_number, std::vector<Person>& records) {
    //cambiar la descompresion del bloque
    
    if (block_number >= total_blocks) {
           // Bloque no existe
           std::cerr << "Error: el bloque " << block_number << " no existe. Total de bloques: " << total_blocks << std::endl;
           return false;
       }

       size_t block_offset = getBlockOffset(block_number);
       data_file.seekg(block_offset);

       uint32_t data_size;
       data_file.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));

       if (data_file.fail()) {
           std::cerr << "Error al leer el tamaño de los datos del bloque " << block_number << std::endl;
           return false;
       }
    
        string serialized_data(data_size,'\0');
        //std::string compressed_data(data_size, '\0'); (cambiar para compresion)
        data_file.read(&serialized_data[0], data_size);

       if (data_file.gcount() != data_size) {
           std::cerr << "No se pudo leer la cantidad esperada de datos del bloque " << block_number << std::endl;
           return false;
       }

       try {
           // Descomprimir el bloque
           stringstream ss(serialized_data);
           //std::stringstream compressed_ss(compressed_data);
           //boost::iostreams::filtering_stream<boost::iostreams::input> in;
           //in.push(boost::iostreams::zlib_decompressor());
           //in.push(compressed_ss);
           boost::archive::binary_iarchive ia(ss);
           ia >> records;
       } catch (const std::exception& e) {
           std::cerr << "Error al descomprimir el bloque " << block_number << ": " << e.what() << std::endl;
           return false;
       }

       return true;
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
    if (index_size == static_cast<size_t>(-1)) {
        std::cerr << "Error al obtener el tamaño del archivo de índice." << std::endl;
        total_blocks = 0;
        return;
    }

    if (index_size % sizeof(BlockIndexEntry) != 0) {
        std::cerr << "Error: Tamaño del archivo de índice no es múltiplo del tamaño de BlockIndexEntry." << std::endl;
        total_blocks = 0;
        return;
    }

    total_blocks = index_size / sizeof(BlockIndexEntry);
    index_file.seekg(0, std::ios::beg);

    //std::cout << "loadBlockIndex: total_blocks = " << total_blocks << std::endl;
}


void DataManager::updateBlockIndex(size_t block_number, size_t block_offset) {
    BlockIndexEntry entry = {block_number, block_offset};
    index_file.seekp(block_number * sizeof(BlockIndexEntry));
    if (index_file.fail()) {
           std::cerr << "Error al hacer seekp en el archivo de índice en el bloque " << block_number << std::endl;
           index_file.clear();
           return;
       }
    
    index_file.write(reinterpret_cast<const char*>(&entry), sizeof(BlockIndexEntry));
    if (index_file.fail()) {
           std::cerr << "Error al hacer seekp en el archivo de índice en el bloque " << block_number << std::endl;
           index_file.clear();
           return;
       }
    
    index_file.flush();
    if (index_file.fail()) {
            std::cerr << "Error al hacer flush del archivo de índice después de escribir el bloque " << block_number << std::endl;
            index_file.clear();
            return;
        }
    
    total_blocks = std::max(total_blocks, block_number + 1);
    
    //cout<<"Actualiazando indice, block_number: "<<block_number<<", block offset "<<block_offset<<", total_blocks: "<<total_blocks <<endl;
}
