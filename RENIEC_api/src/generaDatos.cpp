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

DataManager::DataManager() = default;

void DataManager::generarCiudadano(size_t count){
    random_device rd;
    mt19937 gen((rd));
    uniform_int_distribution<uint32_t> dist(10000000,99999999);
}
