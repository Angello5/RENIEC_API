#ifndef BSTARTREE_H
#define BSTARTREE_H

#include "bufferPool.h"
#include "persona.h"

class BStarTree {
public:
    BStarTree(BufferPool& buffer_pool);
    ~BStarTree();

    void insert(uint32_t key, size_t record_offset);
    bool search(uint32_t key, size_t& record_offset);
    void remove(uint32_t key);

private:
    BufferPool& buffer_pool;
    size_t root_page_id;

    void insertNonFull(size_t page_id, uint32_t key, size_t record_offset);
    void splitChild(size_t parent_page_id, size_t child_index, size_t child_page_id);
};

#endif // BSTARTREE_H
