//
//  generaDatos.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 18/10/24.
//
#include "persona.h"
#include "generaDatos.h"
#include <fstream>
#include <random>
#include <boost/serialization/vector.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

using namespace std;

DataManager::DataManager() = default;

void DataManager::generarCiudadano(size_t count){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dist(10000000,99999999);
    
    citizens.clear();
    citizens.reserve(count);
    
    for(size_t i = 0; i < count; i++){
        uint32_t dni = dist(gen);
        string nombre = generateRandomNombre(10) + " " + generateRandomNombre(10);
        string nacionalidad = "Peruano(a)";
        string birthPlace = generateRandomNombre(8);
        string direccion = generateRandomNombre(20) + "Lima";
        string telefono = "9" + generateRandomNombre(8);
        string correo = generateRandomNombre(6) + "@hotmail.com";
        string estadoCivil = (i % 2 == 0) ? "Soltero" : "Casado";
        
        citizens.emplace_back(dni, nombre, nacionalidad, birthPlace, direccion, telefono, correo, estadoCivil);
    }
}

void DataManager::guardarArchivo(const string &filename){
    ofstream ofs(filename, ios::binary);
    boost::iostreams::filtering_ostream out;
    out.push(boost::iostreams::zlib_compressor()); // Aplica compresión usando zlib
    out.push(ofs);
    
    boost::archive::binary_oarchive oa(out);
    oa << citizens;
}

void DataManager::cargarArchivo(const string &filename){
    ifstream ifs(filename, ios::binary);
    if(ifs){
        boost::archive::binary_iarchive ia(ifs);
        ia >> citizens;
    }
}

void DataManager::guardarEnChunks(const string &filename, size_t chunkSize){
    ofstream ofs(filename, ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    
    size_t totalRecords = citizens.size();
       for (size_t i = 0; i < totalRecords; i += chunkSize) {
           size_t end = std::min(i + chunkSize, totalRecords);
           std::vector<CitizenData> chunk(citizens.begin() + i, citizens.begin() + end);
           oa << chunk;
       }
   }

uint32_t DataManager::generateRandomDNI(){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dist(10000000, 99999999);
    return dist(rd);
}

string DataManager::generateRandomNombre(size_t length){
    static const char charset[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, sizeof(charset) - 2);

        string result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            result += charset[dist(gen)];
        }
        return result;
}
