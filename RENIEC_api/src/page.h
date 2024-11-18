//
//  page.h
//  RENIEC_api
//
//  Created by Angello Llerena on 11/11/24.
//
#ifndef PAGE_H
#define PAGE_H

#include "dataManager.h"
#include <cstddef>
#include <cstdint>
#include <cmath>

const uint32_t MAX_KEYS = 120;
const uint32_t PAGE_SIZE = 4096; //tam de pagina 4kb
const uint32_t MIN_KEYS = static_cast<uint32_t>(std::ceil((2.0 * MAX_KEYS) / 3.0));


struct Page {
    bool is_leaf;
    uint32_t num_keys;
    IndexEntry entries[MAX_KEYS];
    uint32_t children[MAX_KEYS + 1]; // Identificadores de páginas hijas
    uint32_t parent;                 // Identificador de la página padre

    // Constructor por defecto
    Page() : is_leaf(true), num_keys(0), parent(0) {
    }

    // Métodos adicionales si los necesitas
};

#endif // PAGE_H

