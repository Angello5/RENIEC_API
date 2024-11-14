//
//  pageManager.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 11/11/24.
//

#include "pageManager.h"
#include <iostream>

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
                num_pages = static_cast<size_t>(file_size) / PAGE_SIZE;
                file.seekg(0, std::ios::beg);
            }
}

PageManager::~PageManager() {
    file.close();
}

size_t PageManager::allocatePage() {
    size_t page_id;
    if (!free_pages.empty()) {
        page_id = *free_pages.begin();
        free_pages.erase(free_pages.begin());
    }else{
        page_id = num_pages++;
    }
    return page_id;
}

void PageManager::readPage(size_t page_id, Page& page) {
    file.seekg(page_id * PAGE_SIZE);
    file.read(reinterpret_cast<char*>(&page), sizeof(Page));
}

void PageManager::writePage(size_t page_id, const Page& page) {
    file.seekp(page_id * PAGE_SIZE);
    file.write(reinterpret_cast<const char*>(&page), sizeof(Page));
}
