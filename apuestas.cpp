#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <semaphore.h>

using namespace std;

// Constantes
const int NUM_CABALLOS = 5;
const int DISTANCIA_META = 100;
const int MAX_AVANCE = 5; // Unidades máximas que puede avanzar un caballo en 1/4 de segundo

// Semáforos
sem_t mutex_cout; // Para imprimir por consola de manera ordenada
sem_t sem_meta; // Para indicar que un caballo ha llegado a la meta

// Variables globales
int posicion_caballos[NUM_CABALLOS] = {0}; // Posición actual de cada caballo
bool carrera_terminada = false; // Bandera para indicar que todos los caballos han llegado a la meta
int caballo_ganador = -1; // Caballo ganador de la carrera
int opcion_apuesta = -1; // Caballo seleccionado para apostar por el usuario
bool gano_apuesta = false; // Indica si el usuario ganó la apuesta
int caballos_llegados = 0; // Número de caballos que han llegado a la meta

// Funciones auxiliares
void avanzarCaballo(int caballo) {
    while (posicion_caballos[caballo] < DISTANCIA_META && !carrera_terminada) {
        // Avanzar una cantidad aleatoria de unidades
        int avance = rand() % MAX_AVANCE + 1;
        posicion_caballos[caballo] += avance;

        // Esperar 1/2 de segundo
        this_thread::sleep_for(chrono::milliseconds(500));

        // Acceder al recurso compartido del circuito
        sem_wait(&sem_meta);
        if (posicion_caballos[caballo] >= DISTANCIA_META) {
            // El caballo ha llegado a la meta
            caballos_llegados++;
            if (caballos_llegados == NUM_CABALLOS) {
                // Fijar la bandera para indicar que todos los caballos han llegado a la meta
                if (caballo_ganador == -1) {
                   // Este caballo es el primer ganador
                   caballo_ganador = caballo;
                }
                carrera_terminada = true;
                sem_post(&sem_meta);
                break;
            }
            sem_post(&sem_meta);
        }
        else {
            sem_post(&sem_meta);
        }
    }
}

// Función que muestra la pista de carreras
void mostrarPista() {
    while (!carrera_terminada) {
        system("clear"); // Limpiar la consola
        for (int i = 0; i < NUM_CABALLOS; i++) {
            cout << "Caballo " << i + 1 << " |";
            for (int j = 0; j < DISTANCIA_META; j++) {
                if (j == posicion_caballos[i]) {
                    cout << ">";
                }
                else {
                    cout << "-";
                }
            }
            cout << endl;
        }
        cout << "----------------------------------------" << endl;

        // IMPRESIÓN DETALLES DE POSICIÓN
        cout << "Detalles de carrera: ";
        for (int i = 0; i < NUM_CABALLOS; i++) {
            cout << "Caballo " << i + 1 << " (" << posicion_caballos[i] << "), ";
        }
        cout << endl;

        cout << "----------------------------------------" << endl;
        // Esperar 1/4 de segundo
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

int main() {
    // Inicializar semáforos
    sem_init(&mutex_cout, 0, 1);
    sem_init(&sem_meta, 0, 1);

    // Pedir al usuario el caballo a apostar
    while (true) {
    cout << "Seleccione el caballo a apostar (1-5): ";
    cin >> opcion_apuesta;
    if (cin.fail() || opcion_apuesta < 1 || opcion_apuesta > NUM_CABALLOS) {
        cout << "Opción inválida, intente nuevamente.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } else {
        break;
    }
}

    // Crear hilos para cada caballo
    srand(time(NULL)); // Semilla para la generación de números aleatorios
    vector<thread> hilos;
    for (int i = 0; i < NUM_CABALLOS; i++) {
        hilos.push_back(thread(avanzarCaballo, i));
    }

    // Mostrar pista de carreras
    mostrarPista();

    // Esperar a que todos los caballos lleguen a la meta
    for (int i = 0; i < NUM_CABALLOS; i++) {
        hilos[i].join();
    }

    this_thread::sleep_for(chrono::milliseconds(500));

    // Anunciar el resultado de la carrera y la apuesta
    sem_wait(&mutex_cout);
    if (caballo_ganador != -1) {
        cout << "El caballo ganador es el " << caballo_ganador + 2 << "!" << endl;
        if (caballo_ganador == opcion_apuesta - 1) {
            cout << "¡Felicidades! ¡Ha ganado la apuesta!" << endl;
            gano_apuesta = true;
        }
        else {
            cout << "Lo siento, ha perdido la apuesta." << endl;
        }
    }
    else {
        cout << "Ha ocurrido un error en la carrera." << endl;
    }
    
    // Liberar recursos
    sem_destroy(&mutex_cout);
    sem_destroy(&sem_meta);

    return 0;
}