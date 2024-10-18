//
//  main.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 18/10/24.
//

#include <iostream>
#include "persona.h"
#include "generaDatos.h"
#include <vector>
#include <string>
#include <cstdint>
#include <chrono>

using namespace std;

int main() {
    size_t count = 33000000;
    string filename = "citizen_data.bin";
    
    DataManager dataManager;
    
    //TIempo de inicio
    auto start = chrono::high_resolution_clock::now();
    
    //generacion y serilizacion
    cout<<"Generadndo datos de ciudadano..."<< endl;
    dataManager.generarCiudadano(count);
    
    cout<<"Guardando los datos generados en el archivo..." << endl;
    dataManager.guardarEnChunks(filename, 1000000);
    
    //tiempo de finilizacion
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duracion = end - start;
    
    cout<<"Generacion y guardado de " << count << " ciudadanos completados en " << duracion.count()<< " segundos." << endl;
 }
