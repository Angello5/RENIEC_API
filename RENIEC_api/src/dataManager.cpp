#include "dataManager.h"
#include <sstream>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <sys/stat.h>

using namespace std;

bool file_exists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

DataManager::DataManager(const std::string& data_filename, const std::string& index_filename, uint32_t records_per_block)
    : data_filename(data_filename), index_filename(index_filename), records_per_block(records_per_block), block_number(0), total_blocks(0) {
        
        data_file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!data_file.is_open()) {
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

            // Abrir archivo de índice
            if (file_exists(index_filename)) {
                // Si el archivo existe, ábrelo sin truncar
                index_file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
            } else {
                // Si el archivo no existe, créalo
                index_file.open(index_filename, std::ios::out | std::ios::binary);
                index_file.close();
                index_file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
            }
            if (!index_file.is_open()) {
                std::cerr << "Error: No se pudo abrir el archivo de índice " << index_filename << std::endl;
                throw std::runtime_error("No se pudo abrir el archivo de índice");
            }

            loadBlockIndex();

            // Inicializar block_number al total de bloques existentes
            block_number = total_blocks;
            std::cout << "Constructor DataManager: block_number = " << block_number << ", total_blocks = " << total_blocks << std::endl;
}



DataManager::~DataManager() {
    if (!block_records.empty()) {
        compressAndWriteBlock();
        block_records.clear();
    }
    data_file.flush();
    data_file.close();
    
    index_file.flush();
    index_file.close();
}


void DataManager::writePerson(const Person& person, uint64_t& out_block_number, uint32_t& out_record_offset_within_block) {
    block_records.push_back(person);
    out_block_number = block_number;
    out_record_offset_within_block = static_cast<uint32_t>(block_records.size() - 1);

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
    
    streamoff block_offset = data_file.tellp();
    
    //cambiar de compresed_data a serialized_data
    uint64_t data_size = serialized_data.size();
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
    cout << "Escribiendo bloque número: " << block_number << " en offset: " << block_offset << std::endl;

    updateBlockIndex(block_number, block_offset);
    
    block_number++;
    cout << "Después de escribir bloque, block_number: " << block_number << ", total_blocks: " << total_blocks << std::endl;

    std::cout << "Tamaño de los datos serializados: " << serialized_data.size() << " bytes." << std::endl;

}

uint64_t DataManager::getBlockOffset(uint64_t block_number) {
    if (block_number >= total_blocks) {
        std::cerr << "Error: el bloque " << block_number << " no existe. Total de bloques: " << total_blocks << std::endl;
        return 0;
    }

    index_file.seekg(block_number * (sizeof(uint64_t) * 2), std::ios::beg);
    if (index_file.fail()) {
        std::cerr << "Error al hacer seekg en el archivo de índice en el bloque " << block_number << std::endl;
        index_file.clear();
        return 0;
    }

    uint64_t stored_block_number;
    index_file.read(reinterpret_cast<char*>(&stored_block_number), sizeof(uint64_t));
    if (index_file.fail()) {
        std::cerr << "Error al leer block_number del archivo de índice en el bloque " << block_number << std::endl;
        index_file.clear();
        return 0;
    }

    uint64_t block_offset;
    index_file.read(reinterpret_cast<char*>(&block_offset), sizeof(uint64_t));
    if (index_file.fail()) {
        std::cerr << "Error al leer block_offset del archivo de índice en el bloque " << block_number << std::endl;
        index_file.clear();
        return 0;
    }

    if (stored_block_number != block_number) {
        std::cerr << "Error: block_number inconsistente en el archivo de índice. Esperado: " << block_number << ", encontrado: " << stored_block_number << std::endl;
        return 0;
    }

    return block_offset;
}


uint64_t DataManager::getTotalBlocks() const {
    return total_blocks;
}

bool DataManager::loadBlock(uint64_t block_number, std::vector<Person>& records) {
    //cambiar la descompresion del bloque
    
    if (block_number >= total_blocks) {
           // Bloque no existe
           std::cerr << "Error: el bloque " << block_number << " no existe. Total de bloques: " << total_blocks << std::endl;
           return false;
       }

       uint64_t block_offset = getBlockOffset(block_number);
        cout << "Cargando bloque: " << block_number << ", block_offset: " << block_offset << std::endl;
       data_file.seekg(block_offset);

       uint64_t data_size;
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

bool DataManager::readPerson(uint64_t block_number, uint32_t record_offset_within_block, Person& person) {
    std::vector<Person> records;
    loadBlock(block_number, records);

    if (record_offset_within_block < records.size()) {
        person = records[record_offset_within_block];
        return true;
    } else {
        return false;
    }
}

void DataManager::updatePerson(uint64_t block_number, uint32_t record_offset_within_block, const Person& person) {
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
        uint64_t block_offset = getBlockOffset(block_number);
        data_file.seekp(block_offset);
        size_t data_size = compressed_data.size();
        data_file.write(reinterpret_cast<char*>(&data_size), sizeof(data_size));
        data_file.write(compressed_data.data(), data_size);
    } else {
        // Manejar el error: posición de registro inválida
    }
}

void DataManager::loadBlockIndex() {
    index_file.seekg(0, std::ios::end);
    streamoff index_size = index_file.tellg();
    if (index_size == static_cast<streamoff>(-1)) {
        std::cerr << "Error al obtener el tamaño del archivo de índice." << std::endl;
        total_blocks = 0;
        return;
    }

    const size_t entry_size = sizeof(uint64_t) * 2;
    if (index_size % entry_size != 0) {
        std::cerr << "Error: Tamaño del archivo de índice no es múltiplo del tamaño de una entrada de índice." << std::endl;
        total_blocks = 0;
        return;
    }

    total_blocks = index_size / entry_size;
    index_file.seekg(0, std::ios::beg);

    std::cout << "loadBlockIndex: total_blocks = " << total_blocks << std::endl;
}



void DataManager::updateBlockIndex(uint64_t block_number, uint64_t block_offset) {
    index_file.seekp(block_number * (sizeof(uint64_t) * 2), std::ios::beg);
    if (index_file.fail()) {
        std::cerr << "Error al hacer seekp en el archivo de índice en el bloque " << block_number << std::endl;
        index_file.clear();
        return;
    }

    index_file.write(reinterpret_cast<const char*>(&block_number), sizeof(uint64_t));
    if (index_file.fail()) {
        std::cerr << "Error al escribir block_number en el archivo de índice en el bloque " << block_number << std::endl;
        index_file.clear();
        return;
    }

    index_file.write(reinterpret_cast<const char*>(&block_offset), sizeof(uint64_t));
    if (index_file.fail()) {
        std::cerr << "Error al escribir block_offset en el archivo de índice en el bloque " << block_number << std::endl;
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

    std::cout << "Actualizando índice, block_number: " << block_number << ", block_offset: " << block_offset << ", total_blocks: " << total_blocks << std::endl;
}
