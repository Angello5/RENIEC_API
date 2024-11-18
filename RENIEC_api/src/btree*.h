#ifndef BSTARTREE_H
#define BSTARTREE_H

#include "bufferPool.h"
#include "persona.h"
#include "dataManager.h"

class BStarTree {
public:
    BStarTree(BufferPool& buffer_pool);
    ~BStarTree();

    void insert(uint32_t key, uint32_t block_number, uint32_t record_offset_within_block);
    bool search(uint32_t key, uint32_t& block_number, uint32_t& record_offset_within_block);
    void remove(uint32_t key);
    void createNewTree();
private:
    BufferPool& buffer_pool;
    uint32_t root_page_id;

    void insertNonFull(uint32_t page_id, const IndexEntry& entry);
    void splitChild(uint32_t parent_page_id, uint32_t child_index, uint32_t child_page_id);
};

#endif // BSTARTREE_H
