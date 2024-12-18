#include "persona.h"
#include "btree*.h"
#include "bufferPool.h"
#include "dataManager.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
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
const uint32_t DNI_MAX = 42999999;
const uint32_t TOTAL_DNIS = DNI_MAX - DNI_MIN + 1;
const uint32_t BLOCK_SIZE = 1 *1024 * 1024;   //1mb
const uint32_t AVERAGE_RECORD_SIZE = 200;
const uint32_t RECORDS_PER_PAGE = 500000;
const string DATA_FILENAME = "/Users/angellollerena/Documents/EDA-trabajoFinal/RENIEC_api/RENIEC_api/data/user.bin";
const string INDEX_FILENAME = "/Users/angellollerena/Documents/EDA-trabajoFinal/RENIEC_api/RENIEC_api/data/block_index.bin";

const vector<string> names = {
    "Juan", "María", "Pedro", "Luis"
};

const vector<string> surnames = {
    "García", "Martínez", "Rodríguez"
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
    {"Loreto", "Maynas", "Iquitos", "Belén"},
};

const vector<string> marital_statuses = {
    "Soltero(a)", "Casado(a)", "Viudo(a)", "Divorciado(a)"
};

uint32_t permuteDNI(uint32_t x){
    x = ((x ^ 61u) ^ (x >> 16u));
        x *= 9u;
        x = x ^ (x >> 4u);
        x *= 0x27d4eb2du;
        x = x ^ (x >> 15u);
        return x;
}
// Función para generar un DNI aleatorio
/*uint32_t generarDni(uint64_t index) {
    if (index >= TOTAL_DNIS) {
            throw std::runtime_error("No se pueden generar más DNIs únicos.");
        }
        uint32_t permuted = permuteDNI(index);
        uint32_t dni = DNI_MIN + permuted % TOTAL_DNIS;
        return dni;
}
*/
// Generador de números de Telefono
string generarPhone() {
    string phone = "9";
    for (int i = 0; i < 8; ++i) {
        // Se añaden los siguientes 8 dígitos
        phone += to_string(rand() % 10);
    }
    return phone;
}

// Generador de números de emails
string generarEmail(const string& name) {
    vector<string> dominios = { "gmail.com","outlook.com", "usil.pe" };
    string dominio = dominios[rand() % dominios.size()];
    return  + "@" + dominio;
}


// Generador de números del Estado Civil
string generarMarital_status() {
    return marital_statuses[rand() % marital_statuses.size()];
}

// Función para generar una persona
Person generarPersona() {
    Person persona;
    //persona.dni = generarDni(index);
    persona.phone = generarPhone();
    persona.name = names[rand() % names.size()];
    persona.surname = surnames[rand() % surnames.size()];
    persona.email = generarEmail(persona.name);
    persona.marital_status = generarMarital_status();

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

// Función para generar datos masivos y cargarlos en el sistema
void generateAndLoadData(BStarTree& tree, DataManager& dataManager, uint64_t num_personas) {
    cout << "Generando y cargando datos..." << endl;
    auto start = chrono::high_resolution_clock::now();
    
    if (num_personas > TOTAL_DNIS) {
            std::cerr << "Error: No es posible generar más de " << TOTAL_DNIS << " DNIs únicos." << std::endl;
        }
    
    vector<uint32_t> dni_list(TOTAL_DNIS);
        for (uint32_t i = 0; i < TOTAL_DNIS; ++i) {
            dni_list[i] = DNI_MIN + i;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(dni_list.begin(), dni_list.end(), gen);
    
    for (uint64_t i = 0; i < num_personas; ++i) {
        Person persona = generarPersona();
        persona.dni = dni_list[i];
        uint64_t block_number;
        uint32_t record_offset_within_block;
        dataManager.writePerson(persona, block_number, record_offset_within_block);
        tree.insert(persona.dni, block_number, record_offset_within_block);

        // Mostrar progreso cada 1 millon registros
        if ((i + 1) % 1000000 == 0) {
            cout << "Progreso: " << (i + 1) << " registros generados." << endl;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Generación y carga completadas en " << duration.count() << " segundos.\n";
    
    return;
}

void printUser(const Person* persona) {
    if (persona) {
        std::cout << "\nDNI: " << persona->dni << "\n"
            << "Nombre: " << persona->name << "\n"
            << "Apellido: " << persona->surname << "\n"
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

void insertUser(BStarTree& btree, DataManager& dataManager) {
    Person persona = {};
    cout << "Ingresa DNI: ";
    cin >> persona.dni;
    cin.ignore(); // Ignora el carácter de nueva línea residual
    
    //verifica si el DNI ya existe
    uint64_t block_number;
    uint32_t record_offset_within_block;
    if (btree.search(persona.dni, block_number, record_offset_within_block)) {
        cout << "El DNI ingresado ya existe en el sistema.\n";
        return;
    }
    
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
    
    auto start = chrono::high_resolution_clock::now();
    dataManager.writePerson(persona, block_number, record_offset_within_block);
    btree.insert(persona.dni, block_number, record_offset_within_block);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Usuario ingresado correctamente.\n" << endl;
    cout << "Tiempo en insertar un nuevo usuario:  " << duration.count();
}

void searchUser(BStarTree& tree, DataManager& dataManager) {
    uint32_t dni;
    cout << "Ingresa DNI a buscar: ";
    while(!(cin >> dni)){
        cout<< "Entrada invalida ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    auto start = chrono::high_resolution_clock::now();
    uint64_t block_number;
    uint32_t record_offset_within_block;
    if (tree.search(dni, block_number, record_offset_within_block)) {
        Person persona;
        if (dataManager.readPerson(block_number, record_offset_within_block, persona)) {
            if (!persona.is_deleted) {
                printUser(&persona);
            } else {
                cout << "Usuario ha sido eliminado.\n";
            }
        } else {
            cout << "Error al leer el registro.\n";
        }
    } else {
        cout << "Usuario no encontrado.\n";
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Tiempo en buscar un usuario:  " << duration.count();
}

void removeUser(BStarTree& tree, DataManager& dataManager) {
    uint32_t dni;
    cout << "Ingresa DNI a eliminar: ";
    cin >> dni;

    auto start = chrono::high_resolution_clock::now();
    uint64_t block_number;
    uint32_t record_offset_within_block;
    if (tree.search(dni, block_number, record_offset_within_block)) {
        Person persona;
        if (dataManager.readPerson(block_number, record_offset_within_block, persona)) {
            persona.is_deleted = true;
            dataManager.updatePerson(block_number, record_offset_within_block, persona);
            cout << "Usuario eliminado correctamente.\n";
        } else {
            cout << "Error al leer el registro.\n";
        }
    } else {
        cout << "Usuario no encontrado.\n";
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Tiempo en eliminar un usuario:  " << duration.count();
}
void imprimirPrimerosRegistros(DataManager& dataManager) {
    auto start = chrono::high_resolution_clock::now();
    uint32_t records_needed = 10;
    uint32_t records_read = 0;
    uint32_t block_number = 0;

    uint64_t total_blocks = dataManager.getTotalBlocks();

    while (records_read < records_needed && block_number < total_blocks) {
        std::vector<Person> records;
        if (!dataManager.loadBlock(block_number, records)) {
            break; // No hay más bloques para leer
        }

        for (uint32_t i = 0; i < records.size() && records_read < records_needed; ++i) {
            const Person& persona = records[i];
            if (!persona.is_deleted) {
                printUser(&persona);
                ++records_read;
            }
        }
        ++block_number;
    }
    if (records_read == 0) {
        std::cout << "No se encontraron registros.\n";
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Tiempo en mostrar los 10 primeros usuarios:  \n" << duration.count();
}

bool dataExiste() {
    std::ifstream dataFile(DATA_FILENAME);
       std::ifstream indexFile(INDEX_FILENAME);
       std::ifstream treePagesFile("/Users/angellollerena/Documents/EDA-trabajoFinal/RENIEC_api/RENIEC_api/data/tree_pages.bin");
       std::ifstream rootFile("/Users/angellollerena/Documents/EDA-trabajoFinal/RENIEC_api/RENIEC_api/data/btree_root.bin");
       return dataFile.good() && indexFile.good() && treePagesFile.good() && rootFile.good();
}
void verificaIndices(DataManager&  data_manager, PageManager& page_manager){
    cout<<"Verificacion de indices: "<<endl;
    data_manager.printBlockIndex();
}

void mostrarMenu( ){
    cout<<"\nMenu\n"
        <<"1. Insertar Usuario\n"
        <<"2. Buscar Usuario\n"
        <<"3. Eliminar Usuario\n"
        <<"4. Imprimir 10 registros\n"
        <<"5. Verifica indices\n"
        <<"6. Exit\n"
        <<"Elige tu opcion: ";
}

int main() {
    try{
        srand(static_cast<unsigned int>(time(0)));
        
        PageManager page_manager("/Users/angellollerena/Documents/EDA-trabajoFinal/RENIEC_api/RENIEC_api/data/tree_pages.bin");
        BufferPool buffer_pool(100, page_manager);   //tamano 100 paginas
        
        BStarTree btree(buffer_pool);
        
        DataManager data_manager(DATA_FILENAME,INDEX_FILENAME,RECORDS_PER_PAGE);
        
        
        size_t num_personas = 1000; // para probar 1k
        //size_t num_personas = 10000;   //para probar 10k
        //size_t num_personas = 100000; // para probar 100k
        //size_t num_personas = 1000000; // para probar 1 millon
        //size_t num_personas = 10000000; // para probar 10 millones
        //size_t num_personas = 33000000; // para probar 33 millones
        
        
        if(!dataExiste()){
            generateAndLoadData(btree, data_manager, num_personas);

            /*
            //Prueba de busqueda
            uint64_t block_number;
            uint32_t record_offset_within_block;
                if (btree.search(dni_a_buscar, block_number, record_offset_within_block)) {
                    Person persona;
                    if (data_manager.readPerson(block_number, record_offset_within_block, persona)) {
                        if (!persona.is_deleted) {
                            printUser(&persona);
                        } else {
                            std::cout << "Usuario ha sido eliminado.\n";
                        }
                    } else {
                        std::cout << "Error al leer el registro.\n";
                    }
                } else {
                    std::cout << "Usuario no encontrado.\n";
                }
            */
        }else{
            cout<<"Los datos ya existen, Se carga desde los archivos."<<endl;
        }

        int opcion;
        do {
            mostrarMenu();
            cin>>opcion;
            switch (opcion) {
                case 1:{
                    insertUser(btree,data_manager);
                    break;
                }
                case 2: {
                    searchUser(btree, data_manager);
                    break;
                }

                case 3: {
                    removeUser(btree, data_manager);
                    break;
                }

                case 4:
                    imprimirPrimerosRegistros(data_manager);
                    break;
                case 5:
                    verificaIndices(data_manager, page_manager);
                    break;
                case 6:
                    cout<<"Salida...\n"<<endl;
                    break;
                default:
                    cout<<"Eleccion invalida, por favor intente de nuevo. \n";
                    break;
            }
        } while (opcion != 6);
        
        return 0;
        
    }catch (const bad_alloc& e){
        cerr <<"Error de asignacion de memoria " << e.what() << endl;
        return EXIT_FAILURE;
    }catch (const exception& e){
        cerr<< "Excepcion capturada en: "<<e.what()<<endl;
        return EXIT_FAILURE;
    }
}
