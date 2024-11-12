//
//  btreeNode.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 18/10/24.
//

#include "btree*.h"

BStarTree::BStarTree(BufferPool& buffer_pool)
    : buffer_pool(buffer_pool) {
    // Crear una raíz vacía
        Page root;
        root.is_leaf = true;
        root.num_keys = 0;
        root_page_id = buffer_pool.page_manager.allocatePage();
        buffer_pool.writePage(root_page_id, root);
}

BStarTree::~BStarTree() {
    buffer_pool.flush();
}

void BStarTree::insert(uint32_t key, size_t record_offset) {
    Page root;
    buffer_pool.readPage(root_page_id, root);

    if (root.num_keys == MAX_KEYS) {
        // La raíz está llena, debemos dividirla
        size_t new_root_page_id = buffer_pool.page_manager.allocatePage();
        Page new_root;
        new_root.is_leaf = false;
        new_root.num_keys = 0;
        new_root.children[0] = root_page_id;

        splitChild(new_root_page_id, 0, root_page_id);

        root_page_id = new_root_page_id;
        buffer_pool.writePage(root_page_id, new_root);

        insertNonFull(root_page_id, key, record_offset);
    } else {
        insertNonFull(root_page_id, key, record_offset);
    }
}


bool BStarTree::search(uint32_t key, size_t& record_offset) {
    size_t page_id = root_page_id;
    while (true) {
        Page page;
        buffer_pool.readPage(page_id, page);

        int i = 0;
        while (i < page.num_keys && key > page.keys[i]) {
            i++;
        }

        if (i < page.num_keys && key == page.keys[i]) {
            // Encontramos la clave
            // Obtener el offset del registro
            // record_offset = page.records[i];
            return true;
        }

        if (page.is_leaf) {
            return false;
        } else {
            page_id = page.children[i];
        }
    }
}


void BStarTree::remove(uint32_t key) {
    // Implementación de remove
}

void BStarTree::insertNonFull(size_t page_id, uint32_t key, size_t record_offset) {
    Page page;
    buffer_pool.readPage(page_id, page);

    int i = page.num_keys - 1;

    if (page.is_leaf) {
        // Mover claves para hacer espacio
        while (i >= 0 && key < page.keys[i]) {
            page.keys[i + 1] = page.keys[i];
            i--;
        }
        page.keys[i + 1] = key;
        // Almacenar el puntero al registro (podrías necesitar un arreglo adicional)
        // page.records[i + 1] = record_offset;
        page.num_keys++;
        buffer_pool.writePage(page_id, page);
    } else {
        // Encontrar el hijo adecuado
        while (i >= 0 && key < page.keys[i]) {
            i--;
        }
        i++;
        Page child;
        buffer_pool.readPage(page.children[i], child);
        if (child.num_keys == MAX_KEYS) {
            splitChild(page_id, i, page.children[i]);
            buffer_pool.readPage(page_id, page);
            if (key > page.keys[i]) {
                i++;
            }
        }
        insertNonFull(page.children[i], key, record_offset);
    }
}


void BStarTree::splitChild(size_t parent_page_id, size_t child_index, size_t child_page_id) {
    Page parent, child, sibling;
    buffer_pool.readPage(parent_page_id, parent);
    buffer_pool.readPage(child_page_id, child);

    size_t sibling_page_id = buffer_pool.page_manager.allocatePage();
    sibling.is_leaf = child.is_leaf;
    sibling.num_keys = MAX_KEYS / 2;

    // Mover la mitad de las claves al hermano
    for (size_t j = 0; j < sibling.num_keys; j++) {
        sibling.keys[j] = child.keys[j + MAX_KEYS / 2 + 1];
        // Mover otros datos si es necesario
    }

    if (!child.is_leaf) {
        for (size_t j = 0; j <= sibling.num_keys; j++) {
            sibling.children[j] = child.children[j + MAX_KEYS / 2 + 1];
        }
    }

    child.num_keys = MAX_KEYS / 2;

    // Insertar la nueva clave en el padre
    for (size_t j = parent.num_keys; j > child_index; j--) {
        parent.keys[j] = parent.keys[j - 1];
        parent.children[j + 1] = parent.children[j];
    }
    parent.keys[child_index] = child.keys[MAX_KEYS / 2];
    parent.children[child_index + 1] = sibling_page_id;
    parent.num_keys++;

    // Escribir las páginas actualizadas
    buffer_pool.writePage(child_page_id, child);
    buffer_pool.writePage(sibling_page_id, sibling);
    buffer_pool.writePage(parent_page_id, parent);
}

