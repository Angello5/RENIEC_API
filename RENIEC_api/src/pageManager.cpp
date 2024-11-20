//
//  pageManager.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 11/11/24.
//

#include "pageManager.h"
#include <iostream>

using namespace std;

PageManager::PageManager(const std::string& filename)
: filename(filename), num_pages(0) {
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!file) {
        // El archivo no existe, crearlo
        file.open(filename, std::ios::out | std::ios::binary);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        num_pages = 0;
    }
    loadPageIndex();
}

PageManager::~PageManager() {
    savePageIndex();
    file.close();
}

uint32_t PageManager::allocatePage() {
    uint32_t page_id;
    if (!free_pages.empty()) {
        page_id = *free_pages.begin();
        free_pages.erase(free_pages.begin());
    } else {
        page_id = num_pages++;
    }
    // Actualizar num_pages si es necesario
    if (page_id >= num_pages) {
        num_pages = page_id + 1;
    }
    return page_id;
}


bool PageManager::readPage(uint32_t page_id, Page& page) {
    try {
            auto it = page_offsets.find(page_id);
            if (it == page_offsets.end()) {
                std::cerr << "No se encontró el page_id " << page_id << " en el índice." << std::endl;
                return false;
            }
            uint64_t offset = it->second;
            file.seekg(offset, std::ios::beg);
            if (file.fail()) {
                std::cerr << "Error al posicionar el puntero de lectura en el archivo." << std::endl;
                return false;
            }
            boost::archive::binary_iarchive ia(file);
            ia >> page;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Excepción al leer la página: " << e.what() << std::endl;
            return false;
        }
}


bool PageManager::writePage(uint32_t page_id, const Page& page) {
    try {
            file.seekp(0, std::ios::end);
            if (file.fail()) {
                std::cerr << "Error al posicionar el puntero de escritura al final del archivo." << std::endl;
                return false;
            }
            uint64_t offset = file.tellp();
            boost::archive::binary_oarchive oa(file);
            oa << page;
            page_offsets[page_id] = offset;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Excepción al escribir la página: " << e.what() << std::endl;
            return false;
        }
    }

void PageManager::savePageIndex(){
    ofstream index_file("data/page_index.bin", ios::binary);
    boost::archive::binary_oarchive oa(index_file);
    oa <<page_offsets;
}
void PageManager::loadPageIndex(){
    ifstream index_file("page_index.bin", ios::binary);
    if (index_file) {
        boost::archive::binary_iarchive ia(index_file);
        ia>>page_offsets;
        
        //actuializa num_pages al maximo de page_id + 1
        for (const auto& entry : page_offsets) {
            num_pages = entry.first + 1;
        }
    }
}
