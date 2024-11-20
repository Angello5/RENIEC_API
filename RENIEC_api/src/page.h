//
//  page.h
//  RENIEC_api
//
//  Created by Angello Llerena on 11/11/24.
//
#ifndef PAGE_H
#define PAGE_H

#include "dataManager.h"
#include <boost/serialization/array.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <cstddef>
#include <cstdint>

const uint32_t MAX_KEYS = 120;
const uint32_t PAGE_SIZE = 4096; //tam de pagina 4kb
const uint32_t MIN_KEYS = static_cast<uint32_t>(std::ceil((2.0 * MAX_KEYS) / 3.0));


struct Page {
    bool is_leaf;
    uint32_t num_keys;
    IndexEntry entries[MAX_KEYS];
    uint32_t children[MAX_KEYS + 1];
    uint32_t parent;

    // Constructor por defecto
    Page() : is_leaf(true), num_keys(0), parent(0) {
        // Inicializar arreglos si es necesario
    }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & is_leaf;
        ar & num_keys;
        ar & boost::serialization::make_array(entries, MAX_KEYS);
        ar & boost::serialization::make_array(children, MAX_KEYS + 1);
        ar & parent;
    }
};
#endif // PAGE_H

