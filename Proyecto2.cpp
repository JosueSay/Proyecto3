// LIBRERIAS
#include <iostream>
#include <pthread.h>
#include <ctime>
#include <cmath>
#include <omp.h>
#include <unistd.h>
using namespace std;

// VARIABLES
// Cantidad de estaciones para pago por "COMPASS"
#define estaciones_compass 3
// Cantidad de estaciones para pago por "EFECTIVO"
#define estaciones_efectivo 3
// Cantidad de estaciones para pago por "COMPASS"
#define tiempo_compass 5
// Cantidad de estaciones para pago por "EFECTIVO"
#define tiempo_efectivo 10

// Cantidad de carros que usan el método de pago "COMPASS" y "EFECTIVO"
int carros_compass, carros_efectivo;
// Hilos encargados para simular el método de pago de los kioscos "COMPASS" y "EFECTIVO"
pthread_t hilo_compass, hilo_efectivo;

/**
 * Estructura que utiliza el hilo para la funcionalidad de la función "atencion"
 *
 * bandera,  identificación del kiosco que simula el hilo (true: COMPASS, false:EFECTIVO)
 * @param cant_carros, cantidad de carros que van al  kiosco
 * @param tiempo_total, tiempo total en atender a los carros
 * @param tiempo_promedio, tiempo promedio en atender a los carros
 */
struct Parametros
{
    bool bandera;
    int cant_carros;
    double tiempo_total;
    double tiempo_promedio;
};

/**
 * Función para actulizar datos de la estructura Parametros
 * @param t1, tiempo total
 * @param t2, tiempo promedio
 * @param t3, tiempo individual
 */
void actualizarTiempos(Parametros *parametros, double t1, double t2)
{
    parametros->tiempo_total = t1;
    parametros->tiempo_promedio = t2;
}

// Función que se ejecutará en el hilo
void *atencion(void *args)
{
    double t1 = 0;
    double t2 = 0;

    // Convierte el argumento de tipo void* nuevamente a Parametros*
    Parametros *parametros = static_cast<Parametros *>(args);

    clock_t start_time = clock(); // Marca de tiempo inicial
    // LOGICA
    if (parametros->bandera)
    {
        #pragma omp parallel for
        for (int i = 0; i < parametros->cant_carros; i++)
        {
            int kisko = i % 3;
            kisko += 1;
            #pragma omp critical
            {
                cout << "El carro: " << i+1 << " es atendido en el kiosco COMPASS numero " << kisko << ".\tSusurro: soy el trabajador (hilo) numero: " << omp_get_thread_num() << endl;
            }
            // Simular el tiempo de pago
            sleep(tiempo_compass);
        }
    }
    else
    {
        #pragma omp parallel for
        for (int i = 0; i < parametros->cant_carros; i++)
        {
            int kisko = i % 3;
            kisko += 1;
            #pragma omp critical
            {
                cout << "El carro: " << i+1 << " es atendido en el kiosco EFECTIVO numero " << kisko << ".\tSusurro: soy el trabajador (hilo) numero: " << omp_get_thread_num() << endl;
            }
            // Simular el tiempo de pago
            sleep(tiempo_efectivo);
        }
    }
    clock_t end_time = clock(); // Marca de tiempo final

    // Actualizar variable de tiempos
    t1 = difftime(end_time, start_time) / CLOCKS_PER_SEC;
    t2 = t1 / parametros->cant_carros;
    // cout << "TIEMPO TOTAL "<<t1<<endl;

    // Actualizar tiempos de estructura
    actualizarTiempos(parametros, t1, t2);

    return nullptr;
}

int main()
{
    // Pedir datos
    cout << "Ingrese el numero de carros que usan compass: ";
    cin >> carros_compass;
    cout << "Ingrese el numero de carros que usan efectivo: ";
    cin >> carros_efectivo;

    // Crear estructuras
    Parametros parametros_compass = {true, carros_compass, 0, 0};
    Parametros parametros_efectivo = {false, carros_efectivo, 0, 0};

    // Crear hilos
    int hiloC = pthread_create(&hilo_compass, NULL, atencion, &parametros_compass);
    int hiloE = pthread_create(&hilo_efectivo, NULL, atencion, &parametros_efectivo);

    // Esperar a que los hilos terminen
    pthread_join(hilo_compass, NULL);  // Esperar a que el hilo de COMPASS termine
    pthread_join(hilo_efectivo, NULL); // Esperar a que el hilo de EFECTIVO termine

    // Mostrar datos del kiosco
    cout << "================"<< endl;
    cout << "||ESTADISTICAS||"<< endl;
    cout << "================"<< endl;
    cout << "TIEMPOS COMPASS | Tiempo total: " << parametros_compass.tiempo_total << " minutos | Tiempo promedio: " << parametros_compass.tiempo_promedio <<" minutos."<<endl;
    cout << "TIEMPOS EFECTIVO | Tiempo total: " << parametros_efectivo.tiempo_total << " minutos | Tiempo promedio: " << parametros_efectivo.tiempo_promedio <<" minutos."<< endl;

    return 0;
}