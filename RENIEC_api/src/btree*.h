#ifndef BSTARTREE_H
#define BSTARTREE_H

#include "bufferPool.h"
#include "persona.h"
#include "dataManager.h"

class BStarTree {
public:
    BStarTree(BufferPool& buffer_pool);
    ~BStarTree();

    void insert(uint32_t key, size_t block_number, size_t record_offset_within_block);
    bool search(uint32_t key, size_t& block_number, size_t& record_offset_within_block);
    void remove(uint32_t key);
    void createNewTree();
private:
    BufferPool& buffer_pool;
    size_t root_page_id;

    void insertNonFull(size_t page_id, const IndexEntry& entry);
    void splitChild(size_t parent_page_id, size_t child_index, size_t child_page_id);
};

#endif // BSTARTREE_H
