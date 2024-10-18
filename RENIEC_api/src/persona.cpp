//
//  persona.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 18/10/24.
//

#include "persona.h"

using namespace std;
// Constructor definition

CitizenData::CitizenData(uint32_t dni, string name, string nationality, string birthPlace,
                         string address, string phone, string email, string civilStatus)
: dni(dni), nombre(move(nombre)), nacionalidad(move(nacionalidad)),
birthPlace(move(birthPlace)), direccion(move(direccion)), telefono(move(telefono)),
correo(move(correo)), estadoCivil(move(estadoCivil)) {}

// Getter definitions
uint32_t CitizenData::getDNI() const {
    return dni;
}

std::string CitizenData::getNombre() const {
    return nombre;
}

std::string CitizenData::getNacionalidad() const {
    return nacionalidad;
}

std::string CitizenData::getBirthPlace() const {
    return birthPlace;
}

std::string CitizenData::getDireccion() const {
    return direccion;
}

std::string CitizenData::getTelefono() const {
    return telefono;
}

std::string CitizenData::getCorreo() const {
    return correo;
}

std::string CitizenData::getEstadoCivil() const {
    return estadoCivil;
}
