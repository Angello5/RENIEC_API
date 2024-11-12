//
//  page.h
//  RENIEC_api
//
//  Created by Angello Llerena on 11/11/24.
//
#ifndef PAGE_H
#define PAGE_H

#include <cstddef>
#include <cstdint>

// Constantes globales (puedes ajustarlas o moverlas a un archivo de configuración)
const size_t MAX_KEYS = 100;
const size_t PAGE_SIZE = 4096; //tam de pagina 4kb

struct Page {
    bool is_leaf;
    size_t num_keys;
    uint32_t keys[MAX_KEYS];
    size_t children[MAX_KEYS + 1]; // Identificadores de páginas hijas
    size_t parent;                 // Identificador de la página padre

    // Constructor por defecto
    Page() : is_leaf(true), num_keys(0), parent(0) {
        // Inicializar arrays si es necesario
    }

    // Métodos adicionales si los necesitas
};

#endif // PAGE_H

