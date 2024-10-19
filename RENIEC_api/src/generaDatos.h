//
//  generaDatos.h
//  RENIEC_api
//
//  Created by Angello Llerena on 18/10/24.
//
#pragma once
#include "persona.h"
#include <vector>
#include <string>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <cstdint>

using namespace std;

class DataManager{
public:
    DataManager();
    void generarCiudadano(size_t count);
    void guardarArchivo(const string &filename);
    void cargarArchivo(const string &filename);
    void guardarEnChunks(const string &filename, size_t chunkSize);
    const std::vector<CitizenData>& getCitizens() const {
        return citizens;
    }

    
private:
    vector<CitizenData> citizens;
    uint32_t generateRandomDNI();
    string generateRandomNombre(size_t length);
};
