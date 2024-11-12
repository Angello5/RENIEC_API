//
//  bufferPool.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 11/11/24.
//
#include "bufferPool.h"
#include <iostream>
#include <algorithm>

BufferPool::BufferPool(size_t capacity, PageManager& page_manager)
    : capacity(capacity), page_manager(page_manager) {
    // inicializacion
}

BufferPool::~BufferPool() {
    flush();
}

//lee el buffer
void BufferPool::readPage(size_t page_id, Page& page) {
    auto it = buffer.find(page_id);
    if (it != buffer.end()) {
        page = it->second.page;  //pagina encontrada en el buffer
    }else{        //se carga desde disco
        if (buffer.size() >= capacity) {
            evictPage();  //reemplaza usando LRU
        }
        page_manager.readPage(page_id, page);
        buffer[page_id] = {page,false};  //agrega pagina al buffer
    }
}

//escribe en el buffer
void BufferPool::writePage(size_t page_id, const Page& page) {
    auto it = buffer.find(page_id);
    if (it != buffer.end()) {
        //actualiza la pagina en el buffer
        it->second.page = page;
        it->second.is_dirty = true;
    }else{
        if (buffer.size() >= capacity) {
            evictPage();
        }
        
        buffer[page_id] = {page, true};   //agrega pagina al buffer
    }
}

void BufferPool::flush() {
    for (auto& entry : buffer) {
            if (entry.second.is_dirty) {
                page_manager.writePage(entry.first, entry.second.page);
                entry.second.is_dirty = false;
            }
        }
}

void BufferPool::evictPage() {
    //encuentra la pagina menos recientemente utilizada
    size_t lru_page_id;
    auto it = buffer.find(lru_page_id);
    if (it != buffer.end()) {
        if (it->second.is_dirty) {
            // Escribir la página a disco si está sucia
            page_manager.writePage(lru_page_id, it->second.page);
            }
        // Remover la página del buffer
        buffer.erase(it);
    }
}

