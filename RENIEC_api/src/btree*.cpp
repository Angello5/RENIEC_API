//
//  btreeNode.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 18/10/24.
//

#include "btree*.h"
#include <iostream>
using namespace std;

BStarTree::BStarTree(BufferPool& buffer_pool)
    : buffer_pool(buffer_pool) {
        std::ifstream root_file("btree_root.bin", std::ios::binary);
        if (root_file.is_open()) {
            root_file.read(reinterpret_cast<char*>(&root_page_id), sizeof(root_page_id));
            root_file.close();

            // Opcional: Verificar que el root_page_id es válido
            // Podrías agregar lógica aquí para asegurarte de que el root_page_id es coherente
        } else {
            // Si el archivo no existe, crear un árbol nuevo
            Page root;
            root.is_leaf = true;
            root.num_keys = 0;
            root_page_id = buffer_pool.allocatePage();
            buffer_pool.writePage(root_page_id, root);
        }
}

BStarTree::~BStarTree() {
    std::ofstream root_file("btree_root.bin", std::ios::binary);
        if (root_file.is_open()) {
            root_file.write(reinterpret_cast<const char*>(&root_page_id), sizeof(root_page_id));
            root_file.close();
        } else {
            cerr << "Error al abrir btree_root.bin para escribir.\n";
        }

        buffer_pool.flush();
}

void BStarTree::insert(uint32_t key, size_t block_number, size_t record_offset_within_block) {
    Page root;
    buffer_pool.readPage(root_page_id, root);

    IndexEntry entry(key, block_number, record_offset_within_block);

    if (root.num_keys == MAX_KEYS) {
        // La raíz está llena, debemos dividirla
        size_t new_root_page_id = buffer_pool.allocatePage();
        Page new_root;
        new_root.is_leaf = false;
        new_root.num_keys = 0;
        new_root.children[0] = root_page_id;

        splitChild(new_root_page_id, 0, root_page_id);

        root_page_id = new_root_page_id;
        buffer_pool.writePage(root_page_id, new_root);

        insertNonFull(root_page_id, entry);
    } else {
        insertNonFull(root_page_id, entry);
    }
}



bool BStarTree::search(uint32_t key, size_t& block_number, size_t& record_offset_within_block) {
    size_t page_id = root_page_id;
    while (true) {
        Page page;
        buffer_pool.readPage(page_id, page);

        int i = 0;
        while (i < page.num_keys && key > page.entries[i].dni) {
            i++;
        }

        if (i < page.num_keys && key == page.entries[i].dni) {
            // Encontramos la clave
            block_number = page.entries[i].block_number;
            record_offset_within_block = page.entries[i].record_offset_within_block;
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

void BStarTree::insertNonFull(size_t page_id, const IndexEntry& entry) {
    Page page;
    buffer_pool.readPage(page_id, page);

    size_t i = page.num_keys - 1;

    if (page.is_leaf) {
        // Mover entradas para hacer espacio
        while (i >= 0 && entry.dni < page.entries[i].dni) {
            page.entries[i + 1] = page.entries[i];
            i--;
        }
        page.entries[i + 1] = entry;
        page.num_keys++;
        buffer_pool.writePage(page_id, page);
    } else {
        // Encontrar el hijo adecuado
        while (i >= 0 && entry.dni < page.entries[i].dni) {
            i--;
        }
        i++;
        Page child;
        buffer_pool.readPage(page.children[i], child);
        if (child.num_keys == MAX_KEYS) {
            splitChild(page_id, i, page.children[i]);
            buffer_pool.readPage(page_id, page);
            if (entry.dni > page.entries[i].dni) {
                i++;
            }
        }
        insertNonFull(page.children[i], entry);
    }
}



void BStarTree::splitChild(size_t parent_page_id, size_t child_index, size_t child_page_id) {
    Page parent, child, sibling;
    buffer_pool.readPage(parent_page_id, parent);
    buffer_pool.readPage(child_page_id, child);

    size_t sibling_page_id = buffer_pool.allocatePage();
    sibling.is_leaf = child.is_leaf;
    sibling.num_keys = MIN_KEYS; // Suponiendo que MIN_KEYS = MAX_KEYS / 2

    // Mover la mitad de las entradas al hermano
    for (size_t j = 0; j < MIN_KEYS; j++) {
        sibling.entries[j] = child.entries[j + MIN_KEYS + 1];
    }

    if (!child.is_leaf) {
        for (size_t j = 0; j <= MIN_KEYS; j++) {
            sibling.children[j] = child.children[j + MIN_KEYS + 1];
        }
    }

    child.num_keys = MIN_KEYS;

    // Insertar la nueva entrada en el padre
    for (size_t j = parent.num_keys; j > child_index; j--) {
        parent.entries[j] = parent.entries[j - 1];
        parent.children[j + 1] = parent.children[j];
    }
    parent.entries[child_index] = child.entries[MIN_KEYS];
    parent.children[child_index + 1] = sibling_page_id;
    parent.num_keys++;

    // Escribir las páginas actualizadas
    buffer_pool.writePage(child_page_id, child);
    buffer_pool.writePage(sibling_page_id, sibling);
    buffer_pool.writePage(parent_page_id, parent);
}


