//
//  bufferPool.h
//  RENIEC_api
//
//  Created by Angello Llerena on 11/11/24.
//
#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include <unordered_map>
#include <cstddef>
#include "pageManager.h"

using namespace std;

class BufferPool {
public:
    BufferPool(uint32_t capacity, PageManager& page_manager);
    ~BufferPool();

    void readPage(uint32_t page_id, Page& page);
    void writePage(uint32_t page_id, const Page& page);
    void flush();
    uint32_t allocatePage();
private:
    uint32_t capacity;
    PageManager& page_manager;
    uint32_t current_time; // Contador global para LRU

    struct BufferEntry {
        Page page;
        bool is_dirty;
        uint32_t last_accessed; // Para LRU
    };

    unordered_map<uint32_t, BufferEntry> buffer;

    void evictPage();
};

#endif // BUFFERPOOL_H

