//
//  persona.cpp
//  RENIEC_api
//
//  Created by Angello Llerena on 18/10/24.
//

#include "persona.h"

using namespace std;
// Constructor definition

CitizenData::CitizenData(uint32_t dni, string nombre, string nacionalidad, string birthPlace,
                         string direccion, string telefono, string correo, string estadoCivil)
: dni(dni), nombre(nombre), nacionalidad(nacionalidad),
birthPlace(birthPlace), direccion(direccion), telefono(telefono),
correo(correo), estadoCivil(estadoCivil) {}

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
