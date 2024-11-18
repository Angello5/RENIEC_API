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
    }else{
        page_id = num_pages++;
    }
    return page_id;
}

bool PageManager::readPage(uint32_t page_id, Page& page) {
    if (!file.is_open()) {
        std::cerr << "Error: El archivo de páginas no está abierto para lectura." << std::endl;
        return false;
    }

    file.seekg(page_id * PAGE_SIZE);
    if (file.fail()) {
        std::cerr << "Error al hacer seekg en la página " << page_id << "." << std::endl;
        file.clear();
        return false;
    }

    file.read(reinterpret_cast<char*>(&page), sizeof(Page));
    if (file.fail()) {
        std::cerr << "Error al leer la página " << page_id << " desde el archivo." << std::endl;
        file.clear();
        return false;
    }

    return true;
}


void PageManager::writePage(uint32_t page_id, const Page& page) {
    //std::cout << "PageManager: escribiendo página " << page_id << " al archivo." << std::endl;

    if (!file.is_open()) {
        std::cerr << "Error: El archivo no está abierto para escribir." << std::endl;
        return;
    }

    file.seekp(page_id * PAGE_SIZE);

    if (file.fail()) {
        std::cerr << "Error al hacer seekp en la página " << page_id << "." << std::endl;
        file.clear();
        return;
    }

    // Ajuste para escribir exactamente PAGE_SIZE bytes
    char buffer[PAGE_SIZE] = {0};
    std::memcpy(buffer, &page, sizeof(Page));

    file.write(buffer, PAGE_SIZE);

    if (file.fail()) {
        std::cerr << "Error al escribir la página " << page_id << " en el archivo." << std::endl;
        file.clear();
        return;
    }

    file.flush();

    if (file.fail()) {
        std::cerr << "Error al hacer flush del archivo después de escribir la página " << page_id << "." << std::endl;
        file.clear();
    }
}
