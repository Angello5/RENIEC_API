#include "persona.h"
//#include "CuckooHashTable.h"
//#include "MappedFile.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/version.hpp>

#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>
#include <set>
#include <cstdint>
#include <cstring>
#include <random>

using namespace std;

const uint32_t DNI_MIN = 10000000;
const uint32_t DNI_MAX = 77999999;
const uint32_t TOTAL_DNIS = DNI_MAX - DNI_MIN + 1;
const uint32_t PRIME = 100000007;

// Cambia la ruta del archivo según sea necesario
//const std::string FILE_PATH = "E:/USIL/SEXTO CICLO (2024-1)/Análisis y Diseño de Algoritmos/Trabajo Final/Archivos";
const std::string FILE_PATH = "/Users/angellollerena/Documents/EDA-trabajofinal/RENIEC_api/RENIEC_api/data/user.bin";

/*
    FUNCIONES QUE GENERAN LOS REGISTROS
        * Primero las leen de archivos txt para luego
          generarlos e insertarlos en un archivo binario
*/

const vector<string> names = {
    "Juan", "María", "Pedro", "Ana", "Luis"
};

const vector<string> surnames = {
    "García", "Martínez", "Rodríguez", "López", "González"
};

struct NationalityBirthplace {
    string nationality;
    string birthplace;
};

const vector<NationalityBirthplace> natPlaces = {
    {"Peruana", "Lima"},
    {"Chilena", "Santiago"},
    {"Argentina", "Buenos Aires"},
    {"Colombiana", "Bogotá"},
    {"Española", "Madrid"},
    {"Estadounidense", "Nueva York"},
    {"Italiana", "Roma"}
};

struct AddressData {
    string departamento;
    string provincia;
    string ciudad;
    string distrito;
};

const vector<AddressData> addresses = {
    {"Lima", "Lima", "Lima", "Miraflores"},
    {"Cusco", "Cusco", "Cusco", "San Blas"},
    {"Arequipa", "Arequipa", "Arequipa", "Yanahuara"},
    {"Piura", "Piura", "Piura", "Catacaos"},
    {"La Libertad", "Trujillo", "Trujillo", "Huanchaco"},
    {"Junín", "Huancayo", "Huancayo", "El Tambo"},
    {"Loreto", "Maynas", "Iquitos", "Belén"},
    {"Puno", "Puno", "Puno", "Juliaca"},
    {"Ica", "Ica", "Ica", "Paracas"},
    {"Tumbes", "Tumbes", "Tumbes", "Zorritos"}
};

const vector<string> marital_statuses = {
    "Soltero(a)", "Casado(a)", "Viudo(a)", "Divorciado(a)", "Separado(a)", "Conviviente"
};

uint32_t permuteDNI(uint32_t index){
    uint64_t permuted = (static_cast<uint64_t>(index) * 48271) % PRIME;
    return static_cast<uint32_t>(permuted);
}
// Función para generar un DNI aleatorio
uint32_t generarDni(uint32_t index) {
    uint32_t permuted = permuteDNI(index);
    uint32_t dni = DNI_MIN + (permuted % TOTAL_DNIS);
    return dni;
}

// Generador de números de Telefono
string generarPhone() {
    string phone = "9";
    for (int i = 0; i < 8; ++i) {
        // Se añaden los siguientes 8 dígitos
        phone += to_string(rand() % 10);
    }
    return phone;
}
/*
vector<string> leerArchivo(const string& nombreArchivo) {
    vector<string> lineas;
    ifstream archivo(nombreArchivo);
    string linea;
    while (getline(archivo, linea)) {
        lineas.push_back(linea);
    }
    return lineas;
}
 */
// Función para eliminar espacios en blanco de una cadena
string removeSpaces(const string& str) {
    string result;
    for (char c : str) {
        if (!isspace(c)) {
            result += c;
        }
    }
    return result;
}

// Función para convertir una cadena a minúsculas
string toLowerCase(const string& str) {
    string result;
    for (char c : str) {
        result += tolower(c);
    }
    return result;
}
// Generador de números de emails
string generarEmail(const string& name) {
    vector<string> dominios = { "gmail.com", "yahoo.com", "outlook.com", "icloud.com", "@usil.pe" };
    string dominio = dominios[rand() % dominios.size()];
    string cleanName = removeSpaces(name);
    string lowerCaseName = toLowerCase(cleanName);
    return lowerCaseName + "@" + dominio;
}


// Generador de números del Estado Civil
string generarMarital_status() {
    vector<std::string> marital_status = { "Soltero(a)", "Casado(a)", "Viudo(a)", "Divorciado(a)", "Separado(a)", "Conviviente" };
    return marital_status[rand() % marital_status.size()];
}


// Funcion que permite seleccionar un registro de manera aleatoria

Address selAddressRandom(const std::vector<std::string>& direcciones) {
    std::string address = direcciones[rand() % direcciones.size()];
    Address ad;
    size_t pos = 0;

    pos = address.find(',');
    ad.departamento = address.substr(0,pos);
    address.erase(0, pos + 1);
    
    pos = address.find(',');
    ad.provincia = address.substr(0,pos);
    address.erase(0, pos + 1);
    
    pos = address.find(',');
    ad.ciudad= address.substr(0, pos);
    address.erase(0, pos + 1);
    
    ad.distrito = address;
    
    return ad;
    
}
pair<string, string> selNatBPRandom(const vector<string>& natPlace) {
    string nl = natPlace[rand() % natPlace.size()];
    size_t pos = nl.find(',');
    string nacionalidad = nl.substr(0, pos);
    string lugarNacimiento = nl.substr(pos + 1);
    return { nacionalidad, lugarNacimiento };
}

// Función para generar una persona o ciudadano de manera aleatoria
Person generarPersona(uint32_t index) {
    Person persona;
    persona.dni = generarDni(index);
    persona.phone = generarPhone();
    persona.name = names[rand() % names.size()];
    persona.surname = surnames[rand() % surnames.size()];
    persona.email = generarEmail(persona.name);
    persona.marital_status = marital_statuses[rand() % marital_statuses.size()];

    const auto& natPlace = natPlaces[rand() % natPlaces.size()];
    persona.birthplace.nationality = natPlace.nationality;
    persona.birthplace.birthplace = natPlace.birthplace;

    const auto& addr = addresses[rand() % addresses.size()];
    persona.address.departamento = addr.departamento;
    persona.address.provincia = addr.provincia;
    persona.address.ciudad = addr.ciudad;
    persona.address.distrito = addr.distrito;

    return persona;
}



void saveData(const vector<Person>& personas, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    boost::iostreams::filtering_stream<boost::iostreams::output> fos;
    fos.push(boost::iostreams::zlib_compressor());
    fos.push(ofs);
    boost::archive::binary_oarchive oa(fos);
    oa << personas;
}

void loadData(std::vector<Person>& personas, const string& filename) {
    ifstream ifs(filename, ios::binary);
    if (!ifs.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return;
        }

    boost::iostreams::filtering_stream<boost::iostreams::input> fis;
    fis.push(boost::iostreams::zlib_decompressor());
    fis.push(ifs);
    boost::archive::binary_iarchive ia(fis);
    try {
            ia >> personas;
        } catch (const boost::archive::archive_exception& e) {
            std::cerr << "Error al leer datos del archivo: " << e.what() << std::endl;
        }

}

void printUser(const Person* persona) {
    if (persona) {
        std::cout << "DNI: " << persona->dni << "\n"
            << "Nombre: " << persona->name << "\n"
            << "Apellido:: " << persona->surname << "\n"
            << "Nacionalidad: " << persona->birthplace.nationality << "\n"
            << "Lugar de Nacimiento: " << persona->birthplace.birthplace << "\n"
            << "Departamento: " << persona->address.departamento << "\n"
            << "Ciudad: " << persona->address.ciudad << "\n"
            << "Provincia: " << persona->address.provincia << "\n"
            << "Distrito: " << persona->address.distrito << "\n"
            << "Telefono: " << persona->phone << "\n"
            << "Correo: " << persona->email << "\n"
            << "Estado Civil: " << persona->marital_status << "\n";
    }
    else {
        cout << "Usuario no encontrado.\n";
    }
}

/*void insertUser(CuckooHashTable& hashTable, std::vector<Person>& personas) {
    Person persona = {};
    cout << "Ingresa DNI: ";
    cin >> persona.dni;
    cin.ignore(); // Ignora el carácter de nueva línea residual

    cout << "Ingresa Nombre: ";
    std::getline(cin, persona.name);

    cout << "Ingresa Apellido: ";
    std::getline(cin, persona.surname);

    cout << "Ingresa Nationality: ";
    std::getline(cin, persona.birthplace.nationality);

    cout << "Ingresa Lugar de nacimiento: ";
    std::getline(cin, persona.birthplace.birthplace);

    cout << "Ingresa el nombre del departamento: ";
    std::getline(cin, persona.address.departamento);

    cout << "Ingresa el nombre de la ciudad: ";
    std::getline(cin, persona.address.ciudad);

    cout << "Ingresa el nombre de la provincia: ";
    std::getline(cin, persona.address.provincia);

    cout << "Ingresa el nombre del distrito: ";
    std::getline(cin, persona.address.distrito);

    cout << "Ingresa Telefono: ";
    std::getline(cin, persona.phone);

    cout << "Ingresa Correo: ";
    std::getline(cin, persona.email);

    cout << "Ingresa estado Civil: ";
    std::getline(cin, persona.marital_status);
    
    size_t memory_position = personas.size();
    personas.push_back(persona);
    auto start = chrono::high_resolution_clock::now();
    hashTable.insert(persona.dni, memory_position);
    //saveData(personas, FILE_PATH); // Usa la ruta definida

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Usuario ingresado correctamente.\n" << endl;
    cout << "Tiempo en insertar un nuevo usuario:  " << duration.count();
}
*/
void imprimirPrimerosRegistros(const vector<Person>& personas) {
    auto start = chrono::high_resolution_clock::now();  // Se inicia la medición del tiempo

    cout << "\n=== Primeros 10 Registros ===" << endl;

    // Asegura que no se excedan los límites del vector
    size_t numRegistros = min(personas.size(), static_cast<size_t>(10));

    for (size_t i = 0; i < numRegistros; ++i) {
        const Person& persona = personas[i];
        cout << "DNI: " << persona.dni << endl;
        cout<<  "NOMBRE: " <<persona.name <<endl;
        cout << "Apellido: " << persona.surname << endl;
        cout << "Nacionalidad: " << persona.birthplace.nationality << endl;
        cout << "Lugar de Nacimiento: " << persona.birthplace.birthplace << endl;
        cout << "Departamento: " << persona.address.departamento << endl;
        cout << "Ciudad: " << persona.address.ciudad << endl;
        cout << "Provincia: " << persona.address.provincia << endl;
        cout << "Distrito: " << persona.address.distrito << endl;
        cout << "Teléfono: " << persona.phone << endl;
        cout << "Correo: " << persona.email << endl;
        cout << "Estado Civil: " << persona.marital_status << endl;
        cout << endl;
    }

    // Se finaliza la medición del tiempo
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Tiempo en mostrar los 10 primeros registros: " << duration.count() << " segundos\n";
}

bool filexiste(const string& filename){
    ifstream file(filename);
    return file.good();
}

void mostrarMenu( ){
    cout<<"\nMenu\n"
        <<"1. Insertar Usuario\n"
        <<"2. Buscar Usuario\n"
        <<"3. Eliminar Usuario\n"
        <<"4. Imprimir los primeros 10 registros\n"
        <<"5. Guadar datos\n"
        <<"6. Exit\n"
        <<"Elige tu opcion: ";
}

int main() {
    try{
        srand(static_cast<unsigned int>(time(0)));
        size_t num_personas = 33000000; // Use the declared variable
        
        bool dataexiste = filexiste(FILE_PATH);
        
        /*
        vector<string> names = leerArchivo("/Users/angellollerena/Downloads/grupoadafinal/grupoadafinal/nombres.txt");
        vector<string> surnames = leerArchivo("/Users/angellollerena/Downloads/grupoadafinal/grupoadafinal/apellidos.txt");
        vector<string> natPlaces = leerArchivo("/Users/angellollerena/Downloads/grupoadafinal/grupoadafinal/nacionalidadLugarNac.txt");
        vector<string> addresses = leerArchivo("/Users/angellollerena/Downloads/grupoadafinal/grupoadafinal/direcciones.txt");
        */
        // Generate data
        vector<Person> personas;
        
        if(dataexiste){
            // Load data from file if exists
            auto start = chrono::high_resolution_clock::now();
            loadData(personas, FILE_PATH); // Usa la ruta definida
            auto end = chrono::high_resolution_clock::now();
            chrono:std::chrono::duration<double> duration= end - start;
            cout << "Data cargada desde la tabla Cuckoo Hash Table en " << duration.count() << " segundos.\n";
        }else{
            personas.reserve(num_personas);  // Reserve memory for the number of personas
            for (uint32_t i = 0; i < num_personas; ++i) {
                Person persona = generarPersona(i);
                personas.push_back(persona);
            }
            auto start = chrono::high_resolution_clock::now();
            saveData(personas, FILE_PATH); // Usa la ruta definida
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;
            cout << "Generados y guardados en " << duration.count() << " segundos.\n";
        }
        
     /*   CuckooHashTable hashTable(61000000, personas);  // Double the size for optimal performance
        
        for (size_t i = 0; i < personas.size(); i++) {
            if(!personas[i].is_deleted){
                hashTable.insert(personas[i].dni, i);
            }
        }
       */
        // Map the file to memory (optional step)
        //MappedFile mappedFile(FILE_PATH); // Usa la ruta definida
        int opcion;
        do {
            mostrarMenu();
            cin>>opcion;
            switch (opcion) {
                case 1:{
                    //insertUser(hashTable, personas);
                    break;
                }
                case 2: {
                    uint32_t dni;
                    cout << "Ingresa DNI a buscar: ";
                    cin >> dni;
                    
                    auto start = chrono::high_resolution_clock::now();
                    //const Person* user_pos = hashTable.search(dni);
                    auto end = chrono::high_resolution_clock::now();
                    chrono::duration<double> duration = end - start;
                    
                   /* if (user_pos != nullptr) {
                        printUser(user_pos);
                    } else {
                        cout << "Usuario no encontrado.\n";
                    }
                    
                    cout << "Busqueda completa en: " << duration.count() << " segundos.\n";
                    */
                    break;
                }

                case 3: {
                    uint32_t dni;
                    cout << "Ingresa DNI a eliminar: ";
                    cin >> dni;
                    
                    auto start = chrono::high_resolution_clock::now();
                   /* if (hashTable.remove(dni)) {
                        // No es necesario eliminarlo del vector 'personas'
                        cout << "Usuario eliminado correctamente." << endl;
                    } else {
                        cout << "Usuario no encontrado." << endl;
                    }
                    auto end = chrono::high_resolution_clock::now();
                    chrono::duration<double> duration = end - start;
                    cout << "Eliminacion completa en " << duration.count() << " segundos.\n";
                    */
                    break;
                }

                case 4:
                    imprimirPrimerosRegistros(personas);
                    break;
                case 5:
                    saveData(personas, FILE_PATH);
                    cout<<"Datos guardados exitosamente";
                case 6:
                    cout<<"Salida... \n"<<endl;
                    break;
                default:
                    cout<<"Eleccion invalida, por favor intente de nuevo. \n";
                    break;
            }
        } while (opcion != 6);
        
        saveData(personas, FILE_PATH);
 
    }catch (const bad_alloc& e){
        cerr <<"Error de asignacion de memoria " << e.what() << endl;
        return EXIT_FAILURE;
    }catch (const exception& e){
        cerr<< "Excepcion capturada en: "<<e.what()<<endl;
        return EXIT_FAILURE;
    }
    return 0;
}
