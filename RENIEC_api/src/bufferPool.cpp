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
    : capacity(capacity), page_manager(page_manager), current_time(0) {
    // inicialización
}


BufferPool::~BufferPool() {
    flush();
}

//lee el buffer
void BufferPool::readPage(size_t page_id, Page& page) {
    current_time++; // Incrementar el contador global
    auto it = buffer.find(page_id);
    if (it != buffer.end()) {
        page = it->second.page;  // Página encontrada en el buffer
        it->second.last_accessed = current_time; // Actualizar last_accessed
    } else { // Se carga desde disco
        if (buffer.size() >= capacity) {
            evictPage();  // Reemplaza usando LRU
        }
        page_manager.readPage(page_id, page);
        buffer[page_id] = {page, false, current_time};  // Agrega página al buffer
    }
}

void BufferPool::writePage(size_t page_id, const Page& page) {
    cout<<"Escribiendo pagina " <<page_id <<"en el buffer"<<endl;
    current_time++; // Incrementar el contador global
    auto it = buffer.find(page_id);
    if (it != buffer.end()) {
        // Actualiza la página en el buffer
        it->second.page = page;
        it->second.is_dirty = true;
        it->second.last_accessed = current_time; // Actualizar last_accessed
    } else {
        if (buffer.size() >= capacity) {
            evictPage();
        }
        buffer[page_id] = {page, true, current_time};   // Agrega página al buffer
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
    // Encuentra la página menos recientemente utilizada (LRU)
    auto lru_it = buffer.begin();
    for (auto it = buffer.begin(); it != buffer.end(); ++it) {
        if (it->second.last_accessed < lru_it->second.last_accessed) {
            lru_it = it;
        }
    }
    // Escribir la página a disco si está sucia
    if (lru_it->second.is_dirty) {
        page_manager.writePage(lru_it->first, lru_it->second.page);
    }
    // Remover la página del buffer
    buffer.erase(lru_it);
}


size_t BufferPool::allocatePage() {
    return page_manager.allocatePage();
}


