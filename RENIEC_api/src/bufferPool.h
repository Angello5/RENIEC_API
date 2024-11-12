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
#include "PageManager.h"

using namespace std;

class BufferPool {
public:
    BufferPool(size_t capacity, PageManager& page_manager);
    ~BufferPool();

    void readPage(size_t page_id, Page& page);
    void writePage(size_t page_id, const Page& page);
    void flush();
    size_t allocatePage();
private:
    size_t capacity;
    PageManager& page_manager;

    struct BufferEntry {
        Page page;
        bool is_dirty;
        size_t last_accessed; // Para LRU
    };

    unordered_map<size_t, BufferEntry> buffer;

    void evictPage();
};

#endif // BUFFERPOOL_H

