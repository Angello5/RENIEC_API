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
        // Intentar cargar el root_page_id desde el archivo
            std::ifstream root_file("/Users/angellollerena/Documents/EDA-trabajofinal/RENIEC_api/RENIEC_api/data/btree_root.bin", std::ios::binary);
        if (root_file.is_open()) {
                if (root_file.read(reinterpret_cast<char*>(&root_page_id), sizeof(root_page_id))) {
                    std::cout << "root_page_id (" << root_page_id << ") cargado desde btree_root.bin." << std::endl;
                } else {
                    std::cerr << "Error al leer root_page_id desde btree_root.bin. Creando un nuevo árbol." << std::endl;
                    createNewTree();
                }
                root_file.close();
            } else {
                std::cout << "btree_root.bin no existe. Creando un nuevo árbol." << std::endl;
                createNewTree();
            }
}

BStarTree::~BStarTree() {
    std::cout << "Destruyendo BStarTree y guardando root_page_id..." << std::endl;
        // Guardar el root_page_id en un archivo
        std::ofstream root_file("/Users/angellollerena/Documents/EDA-trabajofinal/RENIEC_api/RENIEC_api/data/btree_root.bin", std::ios::binary);
        if (root_file.is_open()) {
            root_file.write(reinterpret_cast<const char*>(&root_page_id), sizeof(root_page_id));
            root_file.close();
            std::cout << "root_page_id (" << root_page_id << ") guardado en btree_root.bin." << std::endl;
        } else {
            std::cerr << "Error al abrir btree_root.bin para escribir.\n";
        }

        buffer_pool.flush();
}

void BStarTree::createNewTree() {
    Page root;
    root.is_leaf = true;
    root.num_keys = 0;
    std::fill(std::begin(root.entries), std::end(root.entries), IndexEntry{});
    std::fill(std::begin(root.children), std::end(root.children), 0);
    root_page_id = buffer_pool.allocatePage();
    buffer_pool.writePage(root_page_id, root);
}


void BStarTree::insert(uint32_t key, uint32_t block_number, uint32_t record_offset_within_block) {
    Page root;
    buffer_pool.readPage(root_page_id, root);

    IndexEntry entry(key, block_number, record_offset_within_block);

    if (root.num_keys == MAX_KEYS) {
        // La raíz está llena, debemos dividirla
        uint32_t new_root_page_id = buffer_pool.allocatePage();
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



bool BStarTree::search(uint32_t key, uint32_t& block_number, uint32_t& record_offset_within_block) {
    uint32_t page_id = root_page_id;
    std::cout << "Iniciando búsqueda en root_page_id: " << root_page_id << std::endl;
    while (true) {
        Page page;
        buffer_pool.readPage(page_id, page);
        //std::cout << "Leyendo página " << page_id << " con " << page.num_keys << " claves." << std::endl;
        //std::cout << "Claves en la página " << page_id << ": ";
        for (uint32_t k = 0; k < page.num_keys; ++k) {
            std::cout << page.entries[k].dni << " ";
        }
        std::cout << std::endl;

        int i = 0;
        while (i < page.num_keys && key > page.entries[i].dni) {
            i++;
        }

        if (i < page.num_keys && key == page.entries[i].dni) {
            // Encontramos la clave
            std::cout << "Clave encontrada en página " << page_id << " en posición " << i << std::endl;
            block_number = page.entries[i].block_number;
            record_offset_within_block = page.entries[i].record_offset_within_block;
            return true;
        }

        if (page.is_leaf) {
            std::cout << "Llegamos a una hoja sin encontrar la clave." << std::endl;
            return false;
        } else {
            std::cout << "Descendiendo a la página hijo " << page.children[i] << std::endl;
            page_id = page.children[i];
        }
    }
}



void BStarTree::remove(uint32_t key) {
    // Implementación de remove
}

void BStarTree::insertNonFull(uint32_t page_id, const IndexEntry& entry) {
    Page page;
    buffer_pool.readPage(page_id, page);
    std::cout << "insertNonFull: Insertando clave " << entry.dni << " en página " << page_id << (page.is_leaf ? " (hoja)" : " (interna)") << std::endl;

    int i = static_cast<int>(page.num_keys) - 1;

    if (page.is_leaf) {
        // Mover entradas para hacer espacio
        while (i >= 0 && entry.dni < page.entries[i].dni) {
            page.entries[i + 1] = page.entries[i];
            i--;
        }
        page.entries[i + 1] = entry;
        page.num_keys++;
        buffer_pool.writePage(page_id, page);
        std::cout << "Clave " << entry.dni << " insertada en página hoja " << page_id << ". Número de claves ahora: " << page.num_keys << std::endl;
    } else {
        // Encontrar el hijo adecuado
        while (i >= 0 && entry.dni < page.entries[i].dni) {
            i--;
        }
        i++;
        std::cout << "Descendiendo al hijo " << i << " de la página " << page_id << std::endl;
        Page child;
        buffer_pool.readPage(page.children[i], child);
        if (child.num_keys == MAX_KEYS) {
            std::cout << "El hijo " << page.children[i] << " está lleno. Necesita dividirse." << std::endl;
            splitChild(page_id, i, page.children[i]);
            buffer_pool.readPage(page_id, page);
            if (entry.dni > page.entries[i].dni) {
                i++;
            }
        }
        insertNonFull(page.children[i], entry);
    }
}


void BStarTree::splitChild(uint32_t parent_page_id, uint32_t child_index, uint32_t child_page_id) {
    Page parent, child, sibling;
    buffer_pool.readPage(parent_page_id, parent);
    buffer_pool.readPage(child_page_id, child);
    
    std::cout << "Dividiendo hijo " << child_page_id << " en índice " << child_index << " del padre " << parent_page_id << std::endl;
    
    uint32_t sibling_page_id = buffer_pool.allocatePage();
    sibling.is_leaf = child.is_leaf;
    sibling.num_keys = MIN_KEYS; // Suponiendo que MIN_KEYS = MAX_KEYS / 2

    // Mover la mitad de las entradas al hermano
    for (uint32_t j = 0; j < MIN_KEYS; j++) {
        sibling.entries[j] = child.entries[j + MIN_KEYS + 1];
    }

    if (!child.is_leaf) {
        for (uint32_t j = 0; j <= MIN_KEYS; j++) {
            sibling.children[j] = child.children[j + MIN_KEYS + 1];
        }
    }

    child.num_keys = MIN_KEYS;

    // Insertar la nueva entrada en el padre
    for (uint32_t j = parent.num_keys; j > child_index; j--) {
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
    
    std::cout << "División completada. Nuevo hermano " << sibling_page_id << " creado." << std::endl;
}


