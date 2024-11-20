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
            } else {
                // El archivo existe, calcular num_pages
                file.seekg(0, std::ios::end);
                std::streampos file_size = file.tellg();
                num_pages = static_cast<uint32_t>(file_size) / PAGE_SIZE;
                file.seekg(0, std::ios::beg);
            }
}

PageManager::~PageManager() {
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


void PageManager::readPage(uint32_t page_id, Page& page) {
    
    std::ifstream file(filename, std::ios::binary);
       if (!file) {
           // Manejar error
       }
       file.seekg(page_id * PAGE_SIZE, std::ios::beg);
       if (file.fail()) {
           // Manejar error
       }
        
    std::cout << "Leyendo página " << page_id << " del disco." << std::endl;
        std::cout << "is_leaf: " << static_cast<int>(page.is_leaf)
                  << ", num_keys: " << page.num_keys << std::endl;

       boost::archive::binary_iarchive ia(file);
       ia >> page;
}


void PageManager::writePage(uint32_t page_id, const Page& page) {
    std::ofstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
        if (!file) {
            // Manejar error
        }
        file.seekp(page_id * PAGE_SIZE, std::ios::beg);
        if (file.fail()) {
            // Manejar error
        }
    std::cout << "Escribiendo página " << page_id << " al disco." << std::endl;
        std::cout << "is_leaf: " << static_cast<int>(page.is_leaf)
                  << ", num_keys: " << page.num_keys << std::endl;
        boost::archive::binary_oarchive oa(file);
        oa << page;
}
