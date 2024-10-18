//
//  persona.h
//  RENIEC_api
//
//  Created by Angello Llerena on 18/10/24.
//

#include <cstdint>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
//#include <boost/serialization/uint32_t.hpp>

using namespace std;
using namespace boost::serialization::access;

class CitizenData{
public:
    CitizenData() = default;
    CitizenData(uint32_t dni, string name, string nacionalidad, string birthPlace, string direccion, string telefono, string correo, string estadoCivil);
    //Getters
    uint32_t getDNI()const;
    string getNombre() const;
    string getNacionalidad() const;
    string getBirthPlace() const;
    string getDireccion() const;
    string getTelefono() const;
    string getCorreo() const;
    string getEstadoCivil() const;
    
private:
    template<class Archive>
    
    void serialize(Archive &ar, const unsigned int version){
        ar & dni;
        ar & nombre;
        ar & nacionalidad;
        ar & birthPlace;
        ar & direccion;
        ar & telefono;
        ar & correo;
        ar & estadoCivil;
    }
    
    uint32_t dni;
    string nombre;
    string nacionalidad;
    string birthPlace;
    string direccion;
    string telefono;
    string correo;
    string estadoCivil;
};
