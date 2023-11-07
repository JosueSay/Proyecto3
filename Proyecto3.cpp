
/*
*----------------------------------------
* Proyecto3.cpp
* ---------------------------------------
* UNIVERSIDAD DEL VALLE DE GUATEMALA
* CC3086 - Programacion de Microprocesadores
* GRUPO 4
* Compilacion : gcc -o "nombre" Proyecto3.cpp -pthread
* ------
* Descripcion: 
*   Simulación de atención a carros en kioscos COMPASS y EFECTIVO con el uso de mutex, variables condicionales y Pthreads
*
*  Este programa simula la atención a carros que utilizan dos métodos de pago (COMPASS y EFECTIVO) en dos kioscos distintos.
*   Utiliza hilos para gestionar la atención simultánea a múltiples carros, registrando tiempos de atención.
*   Comando para ejecutar el programa: "g++ -o nombre_ejecutable Proyecto3.cpp -lpthread"
* ---------------------------------------
* Simulacion de estaciones LA VAS  Proyecto 3
*----------------------------------------*/


// LIBRERIAS
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <unistd.h>
#include <ctime>

using namespace std;

// CONSTANTES
#define estaciones_compass 3            // Estaciones para atender pagos por compass
#define estaciones_efectivo 3           // Estaciones para atender pagos por efectivo
#define tiempo_compass 5                // Tiempo en atender carros con pago compass
#define tiempo_efectivo 10              // Tiempo en atender carros con pago efectivo


struct Kiosco {
    int id;
    bool esCompass;
    int carrosAtendidos;
    double tiempo_total;
    double tiempo_promedio;

    // Constructor
    Kiosco(int i, bool ec) {
        id = i;
        esCompass = ec;
        carrosAtendidos = 0;
        tiempo_total = 0;
        tiempo_promedio = 0;
    }
};


// MUTEX Y VARIABLES GLOBALES
pthread_mutex_t mutex_compass;          // Sincronización para atención de carros compass
pthread_mutex_t mutex_efectivo;         // Sincronización para atención de carros efectivo
vector<Kiosco> kioscos_compass;         // Listado de kioscos paa atender a los carros compass
vector<Kiosco> kioscos_efectivo;        // Listado de kioscos paa atender a los carros efectivo
int carros_compass;                     // Carros que pagan con compass
int carros_efectivo;                    // Carros que pagan con efectivp




/**
 * Función para actulizar datos de la estructura Parametros
 * @param t1, tiempo total
 * @param t2, tiempo promedio
 * @param t3, tiempo individual
 */
void actualizarTiempos(Kiosco *kiosco, double t1, double t2)
{
    kiosco->tiempo_total = t1;
    kiosco->tiempo_promedio = t2;
}


/**
 * Función atencion
 * Función que ejecutan los hilos para atender a los carros
 * @param args, estructura de los kioscos
*/
void* atencion(void* args) {
    double t1 = 0;
    double t2 = 0;
    Kiosco* kiosco = static_cast<Kiosco*>(args); // Convierte el argumento de tipo void* nuevamente a Kiosco*
    
    clock_t start_time = clock(); // Marca de tiempo inicial
    while (true) {
        pthread_mutex_t* mutex = kiosco->esCompass ? &mutex_compass : &mutex_efectivo;  // Obtener el tipo de mutex
        int* carros = kiosco->esCompass ? &carros_compass : &carros_efectivo;           // Obtener el puntero de los carros por atender 
        int tiempo = kiosco->esCompass ? tiempo_compass : tiempo_efectivo;              // Obtener el tiempo para atender a los carros

        // Sección crítica
        pthread_mutex_lock(mutex);

        // Hay carro por atender
        if (*carros > 0) {
            // Registrar un carro atendido
            *carros -= 1;
            kiosco->carrosAtendidos += 1;
            cout << "El kiosco " << kiosco->id << " (" << (kiosco->esCompass ? "Compass" : "Efectivo") << ") atendio un carro. Carros restantes: " << *carros << endl;
            // Simula la atención del carro
            sleep(tiempo); // Simula el tiempo de atención
            
        // No hay carros por atender
        } else {
            // Liberar el mutex y terminar el hilo
            pthread_mutex_unlock(mutex);
            break;
        }
        
        pthread_mutex_unlock(mutex);
    }
    
    clock_t end_time = clock(); // Marca de tiempo final
    
    // Actualizar variable de tiempos
    t1 = difftime(end_time, start_time) / CLOCKS_PER_SEC;
    t2 = (kiosco->carrosAtendidos != 0) ? t1 / kiosco->carrosAtendidos : t1;
    actualizarTiempos(kiosco, t1, t2);

    return nullptr;
}

int main() {
    // Pedir datos
    cout << "Ingrese el numero de carros que usan compass: ";
    cin >> carros_compass;
    cout << "Ingrese el numero de carros que usan efectivo: ";
    cin >> carros_efectivo;

    // Inicializar mutexes
    pthread_mutex_init(&mutex_compass, nullptr);
    pthread_mutex_init(&mutex_efectivo, nullptr);

    // Inicializar kioscos
    for (int i = 0; i < estaciones_compass; ++i) {
        kioscos_compass.emplace_back(i + 1, true);
    }

    for (int i = 0; i < estaciones_efectivo; ++i) {
        kioscos_efectivo.emplace_back(i + 1 + estaciones_compass, false);
    }

    // Crear hilos para cada kiosco
    vector<pthread_t> hilos;
    for (auto& kiosco : kioscos_compass) {
        pthread_t hilo;
        pthread_create(&hilo, nullptr, atencion, &kiosco);
        hilos.push_back(hilo);
    }
    for (auto& kiosco : kioscos_efectivo) {
        pthread_t hilo;
        pthread_create(&hilo, nullptr, atencion, &kiosco);
        hilos.push_back(hilo);
    }

    // Esperar a que todos los hilos terminen
    for (auto& hilo : hilos) {
        pthread_join(hilo, nullptr);
    }

    // Al finalizar todos los hilos, calcular tiempo total y tiempo promedio
    double tiempo_total_compass = 0;
    double tiempo_total_efectivo = 0;
    int carros_atendidos_compass = 0;
    int carros_atendidos_efectivo = 0;

    // Recorrer los kioscos para calcular el tiempo total y el número de carros atendidos
    for (const auto& kiosco : kioscos_compass) {
        tiempo_total_compass += kiosco.tiempo_total;
        carros_atendidos_compass += kiosco.carrosAtendidos;
    }

    for (const auto& kiosco : kioscos_efectivo) {
        tiempo_total_efectivo += kiosco.tiempo_total;
        carros_atendidos_efectivo += kiosco.carrosAtendidos;
    }

    // Calcular tiempo promedio
    double tiempo_promedio_compass = carros_atendidos_compass > 0 ? tiempo_total_compass / carros_atendidos_compass : 0;
    double tiempo_promedio_efectivo = carros_atendidos_efectivo > 0 ? tiempo_total_efectivo / carros_atendidos_efectivo : 0;

    // Mostrar datos del kiosco
    cout << "\n================"<< endl;
    cout << "||ESTADISTICAS||"<< endl;
    cout << "================"<< endl;
    // Mostrar resultados
    cout << "Tiempo total de atencion para carros con pago Compass: " << tiempo_total_compass << " minutos. \tTiempo promedio de atencion por carro con pago Compass: "<< tiempo_promedio_compass << " minutos." << endl;
    cout << "Tiempo total de atencion para carros con pago Efectivo: " << tiempo_total_efectivo << " minutos. \tTiempo promedio de atencion por carro con pago Efectivo: " << tiempo_promedio_efectivo << " minutos." << endl;

    // Destruir mutexes
    pthread_mutex_destroy(&mutex_compass);
    pthread_mutex_destroy(&mutex_efectivo);

    return 0;
}