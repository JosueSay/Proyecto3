/*
   Simulación de atención a carros en kioscos COMPASS y EFECTIVO mediante hilos en C++, mutex y variables condicionales

   Este programa simula la atención a carros que utilizan dos métodos de pago (COMPASS y EFECTIVO) en dos kioscos distintos.
   Utiliza hilos para gestionar la atención simultánea a múltiples carros, registrando tiempos de atención.
*/
//Librerías
#include <iostream>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
using namespace std;

// Definición de variables de estaciones y tiempos para COMPASS y EFECTIVO
#define estaciones_compass 3
#define estaciones_efectivo 3
#define tiempo_compass 5
#define tiempo_efectivo 10

// Variables para la cantidad de carros que utilizan cada método de pago
int carros_compass, carros_efectivo;
// Hilos para simular el proceso de atención en los kioscos COMPASS y EFECTIVO
pthread_t hilo_compass, hilo_efectivo;
// Mutex y variables de condición para sincronización
pthread_mutex_t mutex;
pthread_cond_t cond_compass, cond_efectivo;

// Variable estática para contar los hilos
static int thread_count = 0;

// Función para obtener el número de hilo único
int dameHilo() {
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mutex);
    int num = thread_count++;
    pthread_mutex_unlock(&mutex);
    return num;
}

// Estructura para almacenar datos de atención a carros
struct Parametros {
    bool bandera; // Identificador de kiosco (COMPASS o EFECTIVO)
    int cant_carros; // Cantidad de carros que utilizarán este método
    double tiempo_total; // Tiempo total de atención
    double tiempo_promedio; // Tiempo promedio de atención por carro
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int carro_actual; // Carro que está siendo atendido
};

// Función para actualizar tiempos en la estructura Parametros
void actualizarTiempos(Parametros *parametros, double t1, double t2) {
    parametros->tiempo_total = t1;
    parametros->tiempo_promedio = t2;
}

// Función que simula la atención a los carros en los kioscos
void *atencion(void *args) {
    // Variables para almacenar tiempos y parámetros
    double t1 = 0;
    double t2 = 0;
    Parametros *parametros = static_cast<Parametros *>(args);

    clock_t start_time = clock(); // Se inicia el reloj

    for (int i = 0; i < parametros->cant_carros; i++) {
        pthread_mutex_lock(&(parametros->mutex));
        while (i == parametros->carro_actual) {
            pthread_cond_wait(&(parametros->cond), &(parametros->mutex));
        }

        parametros->carro_actual = i;

        pthread_mutex_unlock(&(parametros->mutex));

        // Simulación de atención en estaciones COMPASS o EFECTIVO
        pthread_mutex_lock(&mutex);
        if (parametros->bandera) {
            cout << "\tSe esta atendiendo el carro " << i << " que paga con COMPASS\t-> Hilo numero: " << dameHilo() << endl;
        } else {
            cout << "\tSe esta atendiendo el carro " << i << " que paga con EFECTIVO\t-> Hilo numero: " << dameHilo() << endl;
        }
        pthread_mutex_unlock(&mutex);

        if (parametros->bandera) {
            sleep(tiempo_compass);
        } else {
            sleep(tiempo_efectivo);
        }

        pthread_mutex_lock(&(parametros->mutex));
        parametros->carro_actual = -1; // Reiniciar el carro actual
        pthread_cond_broadcast(&(parametros->cond)); // Señalizar a otros hilos
        pthread_mutex_unlock(&(parametros->mutex));
    }

    clock_t end_time = clock();
    t1 = difftime(end_time, start_time) / CLOCKS_PER_SEC;
    t2 = t1 / parametros->cant_carros;

    actualizarTiempos(parametros, t1, t2); // Actualizar tiempos en la estructura

    return nullptr;
}

int main() {
    cout << "Ingrese el numero de carros que usan compass: ";
    cin >> carros_compass;
    cout << "Ingrese el numero de carros que usan efectivo: ";
    cin >> carros_efectivo;

    // Crear estructuras para COMPASS y EFECTIVO
    Parametros parametros_compass = {true, carros_compass, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, -1};
    Parametros parametros_efectivo = {false, carros_efectivo, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, -1};

    // Crear hilos para COMPASS y EFECTIVO
    pthread_create(&hilo_compass, NULL, atencion, &parametros_compass);
    pthread_create(&hilo_efectivo, NULL, atencion, &parametros_efectivo);

    // Esperar a que los hilos terminen su ejecución
    pthread_join(hilo_compass, NULL);
    pthread_join(hilo_efectivo, NULL);

    // Mostrar estadísticas de tiempo de atención
    cout << "================" << endl;
    cout << "||ESTADISTICAS||" << endl;
    cout << "================" << endl;
    cout << "TIEMPOS COMPASS | Tiempo total: " << parametros_compass.tiempo_total << " minutos | Tiempo promedio: " << parametros_compass.tiempo_promedio << " minutos." << endl;
    cout << "TIEMPOS EFECTIVO | Tiempo total: " << parametros_efectivo.tiempo_total << " minutos | Tiempo promedio: " << parametros_efectivo.tiempo_promedio << " minutos." << endl;

    return 0;
}
