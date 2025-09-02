////PARCHÍS JAVIER SARAS Y DANIEL GONZÁLEZ////
//Se ha hecho la parte opcional de 1 jugador vs 3 máquinas con dos comportamientos distintos de los jugadores controlados por máquina 

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <fstream>
#ifdef _WIN32
#include <Windows.h>
#undef max
#endif
using namespace std;

typedef enum { Amarillo, Azul, Rojo, Verde, Ninguno, Gris } tColor;
const int NUM_JUGADORES = 4;
const int NUM_FICHAS = 6;
const int NUM_CASILLAS = 68;
typedef int tFichas[NUM_FICHAS];

struct tJugador {
    tColor colorJugador;
    tFichas fichas; 
};

typedef tJugador tJugadores[NUM_JUGADORES];

struct tCasilla {
    tColor calle1;
    tColor calle2;
};

typedef tCasilla tCasillas[NUM_CASILLAS]; //Guarda el color que hay en cada casilla

struct tJuego {
    tCasillas casillas;
    tJugadores jugadores;
    tColor jugadorTurno;
    int tirada;
    int premio = 0;
    int seises = 0;
};

//FUNCIONES DE LA PARTE OBLIGATORIA

void iniciar(tJuego& juego); //Inicializa el generador de números aleatorios, las fichas, el tablero, el color del fondo y el número del jugador que tiene el primer turno 
void iniciaColores();
void mostrar(const tJuego& juego);
void setColor(tColor color);
bool esSeguro(int casilla); //Comprueba si la casilla es un seguro
int salidaJugador(tColor jugador);
int zanataJugador(tColor jugador);
string colorACadena(tColor color); //Asigna color a los jugadores
int cuantasEn(const tJugador& jugador, int casilla); //Comprueba el número de fichas en una casilla
int primeraEn(const tJugador& jugador, int casilla); //Menor índice de ficha del jugador que está en esa casilla (-1 si no hay ninguna ahí)
int segundaEn(const tJugador& jugador, int casilla); //Mayor índice de ficha del jugador que está en esa casilla (-1 si no hay ninguna ahí)
void cargar(tJuego& juego);
void saleFicha(tJuego& juego); //Sale ficha de la casa de un jugador (la de menor índice)
void aCasita(tJuego& juego, int casilla); //Manda a casa a la última ficha que llego a la casilla, la que esta en la calle2. En caso de que ambas fichas de la casilla sean del mismo jugador, se irá a casa la de mayor índicebool procesa5(tJugadores jugadores, int numJugador, int& premio, bool& pasaTurno, tCasillas calle1, tCasillas calle2); //Procesa todo lo necesario en caso de sacar un 5
bool procesa5(tJuego& juego, bool& pasaTurno); //Procesa todo lo necesario en caso de sacar un 5
bool puente(const tJuego& juego, int casilla); //Comprueba si en la casilla hay un puente que no se pueda superar
bool puedeMover(const tJuego& juego, int ficha, int& casilla); //Comprobaciones de si un movimiento es posible de realizar
void mover(tJuego& juego, int ficha, int casilla); //Ejecuta el movimiento de la ficha tras las comprobaciones necesarias
void abrirPuente(tJuego& juego, int casilla, int casilla2, int& ultimaFichaMovida);
bool procesa6(tJuego& juego, bool& pasaTurno, int& ultimaFichaMovida);
bool todasEnMeta(const tJugador& jugador);
bool jugar(tJuego& juego, bool& fin, int& ultimaFichaMovida, tColor jugadorNoMaquina); // La función devolverá true si ha de pasar turno y false si no
void cargar(tJuego& juego, ifstream& archivo);

// FUNCIONES DE LA PARTE OPCIONAL
int seleccionaJugHuman();
int puedeComer(tJuego& juego, int& casilla); // comprobará si alguna ficha de las que se pueden mover comería una ficha de otro 
int aMeta(tJuego& juego, int& casilla); // comprobará si alguna ficha puede llegar a la meta
int aSeguro(tJuego& juego, int& casilla); // comprobará si alguna ficha puede llegar a una casilla segura 
int huir(tJuego& juego, int& casilla); // comprobará si alguna ficha tiene fichas enemigas cerca y debería ser movida para huir del peligro
int primeraPosible(tJuego& juego, int& casilla); // devolverá el índice de la primera ficha que se puede mover, preferiblemente que no esté en un seguro.


const string Archivo = "18.txt";
const bool Debug = false;
const bool TiradaAleatoria = false;
const bool JuegoOpcional = true;


int main() {
    bool fin = false, jugadaForzosa = false, pasaTurno = true, jugada = false, puede = false, archivoAbierto;
    int  casilla, ficha, ultimaFichaMovida = -1;
    tColor jugadorNoMaquina = Ninguno;
    ifstream archivo;
    tJuego juego;
    system("chcp 1252");
    system("cls");
    //A�adir si os sale el fondo en negro: system("color F0");
    system("color F0");
    if (JuegoOpcional) {
        cout << endl << "Vas a jugar a la versión del parchís 1 vs 3 máquinas" << endl << endl,
            jugadorNoMaquina = tColor(seleccionaJugHuman());
    }

    iniciaColores();
    iniciar(juego);
    if (Debug) {
        archivo.open(Archivo);
        if (archivo.is_open()) {
            cargar(juego, archivo);
        }
        else
            cout << "No se ha podido abrir el archivo" << endl;
    }
    while (!fin) {
        system("cls");
        mostrar(juego);
        setColor(juego.jugadorTurno);
        cout << "Turno para el jugador " << colorACadena(juego.jugadorTurno) << endl;
        if (archivo.is_open() && juego.premio == 0) {
            archivo >> juego.tirada;
            archivo.get();
            if (juego.tirada != -1)
                cout << "Sale un " << juego.tirada << endl;
            if (juego.tirada == -1) //Si se lee el centinela se cierra el archivo
                archivo.close();
        }
        if (juego.premio == 0 && !archivo.is_open()) {
            if (!TiradaAleatoria) {
                cout << "Dame tirada (0 para salir): ";
                cin >> juego.tirada;
                cin.get(); // Saltar el Intro (para que la pausa funcione bien en modo Debug)
                cout << "Sale un " << juego.tirada << endl;
            }
            else if (TiradaAleatoria) {
                juego.tirada = rand() % 6 + 1;
                cout << "Sale un " << juego.tirada << endl;
            }
        }
        if (juego.premio > 0) {
            cout << "Ha de contar " << juego.premio << "!" << endl;
            juego.tirada = juego.premio;
            juego.premio = 0;
        }
        jugada = false;
        pasaTurno = true;
        if (juego.tirada < 6)
            juego.seises = 0;

        if (juego.jugadores[juego.jugadorTurno].fichas[0] == -1 && juego.jugadores[juego.jugadorTurno].fichas[1] == -1 && juego.jugadores[juego.jugadorTurno].fichas[2] == -1 && juego.jugadores[juego.jugadorTurno].fichas[3] == -1 && juego.tirada != 5)
            cout << "El jugador tiene todas las fichas en casa..." << endl;
        if (juego.tirada == 5)
            jugada = procesa5(juego, pasaTurno);
        else if (juego.tirada == 6)
            jugada = procesa6(juego, pasaTurno, ultimaFichaMovida);
        if (!jugada)
            pasaTurno = jugar(juego, fin, ultimaFichaMovida, jugadorNoMaquina);
        if (todasEnMeta(juego.jugadores[juego.jugadorTurno]))
            fin = true;
        if (pasaTurno) {
            juego.jugadorTurno = tColor((juego.jugadorTurno + 1) % NUM_JUGADORES);
            juego.seises = 0;
            ultimaFichaMovida = -1;
        }
        system("pause");
    }
    if (fin && todasEnMeta(juego.jugadores[juego.jugadorTurno])) {
        mostrar(juego);
        setColor(juego.jugadorTurno);
        cout << "Ha ganado el jugador " << colorACadena(juego.jugadorTurno) << "!!!!" << endl;
    }
    return 0;
}

void iniciar(tJuego& juego) {
    for (int i = 0; i < NUM_JUGADORES; i++)
        for (int j = 0; j < NUM_FICHAS; j++) {
            juego.jugadores[i].fichas[j] = -1;
            juego.jugadores[i].colorJugador = tColor(i);
        }
    for (int i = 0; i < NUM_CASILLAS; i++) {
        juego.casillas[i].calle1 = Ninguno;
        juego.casillas[i].calle2 = Ninguno;
    }
    srand(time(NULL));
    juego.jugadorTurno = tColor(0 + rand() % (NUM_JUGADORES - 0));
}

bool esSeguro(int casilla) {
    bool seguro = false;
    if (((casilla % 17) == 0) || ((casilla % 17) == 5) || ((casilla % 17) == 12))
        seguro = true;
    return seguro;
}

int salidaJugador(tColor jugador) {
    int salida = -1;
    switch (jugador) {
    case Amarillo:
        salida = 5;
        break;
    case Azul:
        salida = 22;
        break;
    case Rojo:
        salida = 39;
        break;
    case Verde:
        salida = 56;
        break;
    }
    return salida;
}

string colorACadena(tColor color) {
    string colorJugador;
    if (color == Rojo)
        colorJugador = "rojo";
    if (color == Verde)
        colorJugador = "verde";
    if (color == Amarillo)
        colorJugador = "amarillo";
    if (color == Azul)
        colorJugador = "azul";

    return colorJugador;
}

int zanataJugador(tColor jugador) {
    int zanata = -1;
    switch (jugador) {
    case Amarillo:
        zanata = 0;
        break;
    case Azul:
        zanata = 17;
        break;
    case Rojo:
        zanata = 34;
        break;
    case Verde:
        zanata = 51;
        break;
    }
    return zanata;
}

int cuantasEn(const tJugador& jugador, int casilla) {
    int cuantas = 0;
    for (int i = 0; i < NUM_FICHAS; i++) {
        if (jugador.fichas[i] == casilla)
            cuantas++;
    }
    return cuantas;
}

int primeraEn(const tJugador& jugador, int casilla) {
    int ficha = -1, i = 0;
    bool encontrada = false;
    while (!encontrada && i < NUM_FICHAS) {
        if (jugador.fichas[i] == casilla) {
            ficha = i;
            encontrada = true;
        }
        else
            i++;
    }
    return ficha;
}

int segundaEn(const tJugador& jugador, int casilla) {
    int ficha = -1, i = NUM_FICHAS - 1;
    bool encontrada = false;
    while (!encontrada && i >= 0) {
        if (jugador.fichas[i] == casilla) {
            ficha = i;
            encontrada = true;
        }
        else
            i--;
    }
    return ficha;
}

void saleFicha(tJuego& juego) {
    int salida = salidaJugador(juego.jugadorTurno), i = primeraEn(juego.jugadores[juego.jugadorTurno], -1); //Menor índice en casa
    bool encontrada = false;
    juego.casillas[salida].calle2 = juego.casillas[salida].calle1;
    juego.casillas[salida].calle1 = juego.jugadorTurno;
    juego.jugadores[juego.jugadorTurno].fichas[i] = salidaJugador(juego.jugadorTurno);
}

void aCasita(tJuego& juego, int casilla) {
    tColor color = juego.casillas[casilla].calle2;
    int ficha = segundaEn(juego.jugadores[color], casilla);
    juego.jugadores[color].fichas[ficha] = -1; // Asigna a la ficha encontrada la posición -1
    juego.casillas[casilla].calle2 = Ninguno;
}

bool procesa5(tJuego& juego, bool& pasaTurno) {
    int ficha = primeraEn(juego.jugadores[juego.jugadorTurno], -1); //Busca la primera ficha en casa
    int salida = salidaJugador(juego.jugadorTurno);
    bool puede;
    juego.premio = 0;
    pasaTurno = true;
    if (ficha != -1) { //Si hay ficha en casa
        if (juego.casillas[salida].calle2 == Ninguno) { //Si la calle2 en la salida no está ocupada se puede sacar ficha
            saleFicha(juego);
            puede = true;
            cout << "Sale de casa una ficha (Premio 0)" << endl;
        }
        else { //Si hay ficha en la calle2
            if (juego.casillas[salida].calle2 == juego.jugadorTurno) //Si es de su color no se puede hacer nada (la de la calle1 también tiene que ser de su color)
                puede = false;
            else { //Si no es de su color
                cout << "Sale de casa una ficha comiendo una ficha del jugador " << colorACadena(juego.casillas[salida].calle2) << " (Premio 20)" << endl;
                aCasita(juego, salida); //Manda a la ficha a casa
                saleFicha(juego);
                puede = true;
                pasaTurno = false;
                juego.premio = 20;
            }
        }
    }
    else
        puede = false;
    if (!puede)
        cout << "No puede salir una ficha de casa" << endl;
    return puede;
}

bool puente(const tJuego& juego, int casilla) { // Comprueba que haya un puente de dos ficha iguales (del mismo color)
    bool existePuente;
    tColor fichaCalle2 = juego.casillas[casilla].calle2;
    tColor fichaCalle1 = juego.casillas[casilla].calle1;
    if ((fichaCalle2 == fichaCalle1) && fichaCalle1 != Ninguno && (casilla >= 0 && casilla <= 67)) //Si hay dos fichas en una misma casilla (y está en la calle) entonces es un puente
        existePuente = true;
    else
        existePuente = false;
    return existePuente;
}

bool puedeMover(const tJuego& juego, int ficha, int& casilla) {
    int contador = 0, casillaAComprobar = juego.jugadores[juego.jugadorTurno].fichas[ficha];
    bool puede = true;
    if (casillaAComprobar == -1) //Si la ficha está en casa no se puede mover
        puede = false;
    else {
        while (puede && contador < juego.tirada) { //Se comprueba casilla a casilla si hay un puente que impida el paso
            casillaAComprobar++;
            if (casillaAComprobar == NUM_CASILLAS) //Si llega al final del tablero
                casillaAComprobar = 0;
            if (casillaAComprobar == (zanataJugador(juego.jugadorTurno) + 1)) //Si la casilla es la siguiente a su zanata
                casillaAComprobar = 101;
            if ((puente(juego, casillaAComprobar) && juego.casillas[casillaAComprobar].calle1 != juego.jugadorTurno) || casillaAComprobar > 108) //Si hay puente que no sea de su color o se pasa de la meta
                puede = false;
            contador++;
        }
    }
    if (casillaAComprobar < 101) { //Para asegurarse de introducir valores válidos en los array
        if (juego.casillas[casillaAComprobar].calle1 != Ninguno && juego.casillas[casillaAComprobar].calle2 != Ninguno) //Si la casilla final tiene dos fichas (no hace falta saber si es seguro, no hay otra opción)
            puede = false;
    }
    if (cuantasEn(juego.jugadores[juego.jugadorTurno], casillaAComprobar) == 2 && casillaAComprobar != 108) //Si en la casilla final hay dos de tus fichas no la puedes ocupar
        puede = false;
    if (puede)
        casilla = casillaAComprobar;
    return puede;
}

void mover(tJuego& juego, int ficha, int casilla) {
    int posInicial = juego.jugadores[juego.jugadorTurno].fichas[ficha];
    if (posInicial >= 0 || posInicial <= NUM_CASILLAS) {
        if (juego.casillas[posInicial].calle1 == juego.jugadorTurno) //Si tienes una ficha en la calle1, la de la calle2 se pasa a la calle1 (si no hay ficha se pasa Ninguno)
            juego.casillas[posInicial].calle1 = juego.casillas[posInicial].calle2;
        juego.casillas[posInicial].calle2 = Ninguno; //En cualquier caso, la calle2 debe pasar a valer Ninguno
        if (casilla < NUM_CASILLAS) {
            if (juego.casillas[casilla].calle1 != Ninguno) { //Si hay alguna ficha en la calle1
                if (juego.casillas[casilla].calle1 == juego.jugadorTurno) //En caso de ser tuya la nueva se queda en calle2
                    juego.casillas[casilla].calle2 = juego.jugadorTurno;
                else { //Si no es tuya
                    if (esSeguro(casilla)) //Si es seguro la tuya se queda en calle2
                        juego.casillas[casilla].calle2 = juego.jugadorTurno;
                    else { //Si no es un seguro te la comes
                        juego.casillas[casilla].calle2 = juego.casillas[casilla].calle1;
                        cout << "Se ha comido una ficha del jugador " << colorACadena(juego.casillas[casilla].calle2) << endl;
                        aCasita(juego, casilla);
                        juego.casillas[casilla].calle1 = juego.jugadorTurno;
                        juego.premio = 20;
                    }
                }
            }
            else //Si no hay ficha en la calle1 la ocupas tú
                juego.casillas[casilla].calle1 = juego.jugadorTurno;
        }
    }
    if (casilla == 108) { //Si la casilla de llegada es la meta
        cout << "La ficha del jugador ha llegado a la meta!" << endl;
        juego.premio = 10;
    }
    juego.jugadores[juego.jugadorTurno].fichas[ficha] = casilla; //Se actualiza la posición de la ficha movida
}

void abrirPuente(tJuego& juego, int casilla, int casilla2, int& ultimaFichaMovida) {
    int ficha = segundaEn(juego.jugadores[juego.jugadorTurno], casilla);
    cout << "Se abre el puente de la casilla " << casilla << endl;
    mover(juego, ficha, casilla2); //Se mueve la ficha de mayor índice que formaba el puente
    ultimaFichaMovida = ficha;
}

bool procesa6(tJuego& juego, bool& pasaTurno, int& ultimaFichaMovida) {
    bool puede;
    int puente1 = -1, puente2 = -1, contador = 0; //Las variables puentes guardan las casillas donde se han detectado dos fichas del mismo color
    int llegada1, llegada2; //Utilizadas si avanzas tirada casillas desde puente1 o puente2 respectivamente
    int fichaMayorIndicePuente1, fichaMayorIndicePuente2; //Guarda el valor de la ficha de mayor índice de cada puente
    bool abrirPuente1 = false, abrirPuente2 = false;  //Comprueba si se pueden mover las fichas de los puentes
    tColor paso;
    juego.seises++;
    for (int i = 0; i < NUM_FICHAS; i++) {
        if (juego.jugadores[juego.jugadorTurno].fichas[i] != -1)
            contador++;
    }
    if (contador == 4) {
        juego.tirada = 7;
        cout << "Como no tiene fichas en casa, cuenta 7!" << endl;
    }
    if (juego.seises == 3) {
        juego.seises = 0;
        puede = true;
        if (ultimaFichaMovida != -1) { //Comprobamos si se ha movido alguna ficha, porque si no es asi, la ultimaFichaMovida no se habría actualizado y valdría -1
            if (juego.jugadores[juego.jugadorTurno].fichas[ultimaFichaMovida] > 100) { //Comprobamos si la ultima ficha movida ha entrado en la subida a meta
                pasaTurno = true;
                cout << "Tercer seis consecutivo... La última ficha movida se salva por estar en la subida a meta!" << endl;
            }
            else {
                if (juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ultimaFichaMovida]].calle2 == Ninguno) { //Si el jugador había caido con la penúltima tirada 6 en una casilla sin fichas
                    paso = juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ultimaFichaMovida]].calle1;
                    juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ultimaFichaMovida]].calle1 = juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ultimaFichaMovida]].calle2;
                    juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ultimaFichaMovida]].calle2 = paso;
                }
                aCasita(juego, juego.jugadores[juego.jugadorTurno].fichas[ultimaFichaMovida]); //En caso de que no se de lo anterior ultimaFichaMovida necesariamente estará en la calle2
                pasaTurno = true;
                cout << "Tercer seis consecutivo... La última ficha movida se va a casa!" << endl;
            }
        }
    }
    else { //Todas las situaciones restantes a si no se trata de la tercera tirada de 6
        pasaTurno = false;
        for (int i = 0; i < NUM_FICHAS; i++) {
            if (puente(juego, juego.jugadores[juego.jugadorTurno].fichas[i])) { //Revisamos para cada ficha del jugador si está en un puente
                if (puente1 != -1 && juego.jugadores[juego.jugadorTurno].fichas[i] != puente1) //Si ya se ha encontrado un puente y esta ficha que está en un puente, no pertenece al puente ya detectado
                    puente2 = juego.jugadores[juego.jugadorTurno].fichas[i];  //Entonces se guardará la posición de este nuevo puente
                else //Aquí se entrará cuando todavía no se hayan detectado puentes (y guardará la posición del primer puente encontrado
                    puente1 = juego.jugadores[juego.jugadorTurno].fichas[i];
            }
        }
        if (puente2 != -1) { //En este caso puente1 también será != -1 (y se tratará del caso de disponer de dos puentes para abrir por tirada 6)
            fichaMayorIndicePuente1 = segundaEn(juego.jugadores[juego.jugadorTurno], puente1);
            fichaMayorIndicePuente2 = segundaEn(juego.jugadores[juego.jugadorTurno], puente2);
            llegada1 = puente1;
            llegada2 = puente2;
            abrirPuente1 = puedeMover(juego, fichaMayorIndicePuente1, llegada1); //Al invocarlas con llegada1 se modifican sus valores avanzando tirada casillas
            abrirPuente2 = puedeMover(juego, fichaMayorIndicePuente2, llegada2);
            if (abrirPuente1 && abrirPuente2) { //Si ambos puentes pueden abrirse, no será un movimiento obligatorio y será el jugador el que decida que puente abrir
                puede = false;
            }
            else if (abrirPuente1 || abrirPuente2) { //Entra si solo puede abrirse uno de los puentes
                if (abrirPuente1) {
                    abrirPuente(juego, puente1, llegada1, ultimaFichaMovida);
                    ultimaFichaMovida = fichaMayorIndicePuente1;
                }
                else { //Aquí entrará si no se puede abrir el primer puente
                    abrirPuente(juego, puente2, llegada2, ultimaFichaMovida);
                    ultimaFichaMovida = fichaMayorIndicePuente2;
                }
                puede = true;
            }
            else {
                cout << "No se puede abrir ninguno de los dos puentes! Se ingora la tirada..." << endl;
                puede = true;
            }
        }
        else if (puente1 != -1) { //Si solo hay un puente
            fichaMayorIndicePuente1 = segundaEn(juego.jugadores[juego.jugadorTurno], puente1);
            llegada1 = puente1;
            abrirPuente1 = puedeMover(juego, fichaMayorIndicePuente1, llegada1); //Al invocarlas con llegada1 se modifican sus valores avanzando tirada casillas
            if (abrirPuente1) {
                abrirPuente(juego, puente1, llegada1, ultimaFichaMovida);
                ultimaFichaMovida = fichaMayorIndicePuente1;
                puede = true;
            }
            else //Si el puente no se puede abrir, dará puede false para entrar en jugar y comprobar si otra ficha se puede mover
                puede = false;
        }
        else
            puede = false;
    }
    return puede;
}

bool todasEnMeta(const tJugador& jugador) {
    bool ganar;
    int contador = 0;
    for (int i = 0; i < NUM_FICHAS; i++) { //Comprueba si la posición de todas las fichas es la meta
        if (jugador.fichas[i] == 108)
            contador++;
    }
    if (contador == 4)
        ganar = true;
    else
        ganar = false;
    return ganar;
}

bool jugar(tJuego& juego, bool& fin, int& ultimaFichaMovida, tColor jugadorNoMaquina) {
    bool pasarTurno = true, puede = false;
    int ficha, casilla, cuantas = 0, fichaAMover = -1;
    for (int f = 0; f < NUM_FICHAS; f++) {
        if (puedeMover(juego, f, casilla)) {
            puede = true;
            cuantas++;
            ficha = f; //Si solo se puede moverse una ficha se guardará en la variable ficha, y si son dos o más fichas esa variable no será utilizada
        }
    }
    if (puede && cuantas != 1) { //Si se puede mover más de una ficha
        if (JuegoOpcional && juego.jugadorTurno != jugadorNoMaquina) { //Si es el turno de la máquina se ejecutan sus funciones
            if (juego.jugadorTurno == Amarillo || juego.jugadorTurno == Rojo) { //Los jugadores Amarillo y Rojo optarán por un juego más agresivo, priorizando dañar al resto de jugadores
                ficha = puedeComer(juego, casilla);
                if (ficha == -1)
                    ficha = aMeta(juego, casilla);
                if (ficha == -1)
                    ficha = aSeguro(juego, casilla);
                if (ficha == -1)
                    ficha = huir(juego, casilla);
                if (ficha == -1)
                    ficha = primeraPosible(juego, casilla); //Necesariamente debe devolver un valor distinto de -1
            }
            else { //Los jugadores Azul y Verde jugarán de forma más pasiva, buscando en todo momento la seguridad de sus propias fichas
                ficha = aSeguro(juego, casilla);
                if (ficha == -1)
                    ficha = huir(juego, casilla);
                if (ficha == -1)
                    ficha = puedeComer(juego, casilla);
                if (ficha == -1)
                    ficha = aMeta(juego, casilla);
                if (ficha == -1)
                    ficha = primeraPosible(juego, casilla); //Necesariamente debe devolver un valor distinto de -1
            }
        }
        else { //Si no es turno de la máquina 
            do { //Para que al menos entre una vez antes de asegurarnos que el jugador introduce un valor correcto
                for (int f = 0; f < NUM_FICHAS; f++) {
                    if (puedeMover(juego, f, casilla)) {
                        cout << f + 1 << ": Va de la casilla " << juego.jugadores[juego.jugadorTurno].fichas[f] << " a la casilla " << casilla << endl;
                    }
                }
                cout << "Ficha a mover (0 para salir): ";
                cin >> ficha;
                cin.get();
                ficha--;
                if (ficha > -1 && ficha < NUM_FICHAS && !puedeMover(juego, ficha, casilla))
                    ficha = -2; //Para que se repita el bucle
            } while (ficha < -1 || ficha > NUM_FICHAS - 1);
        }
        if (ficha == -1) // Ponemos -1 porque si el jugador quiere finalizar la partida escribirá 0, y luego se resta 1, así que quedará -1
            fin = true;
        else if (puedeMover(juego, ficha, casilla)) { //Para que nos devuelva el valor de casilla
            mover(juego, ficha, casilla);
            cout << "Premio: " << juego.premio << endl;
            ultimaFichaMovida = ficha;
        }
    }
    else if (puede && cuantas == 1) { //Si solo se puede mover una ficha el movimiento es forzoso
        cout << "Solo se puede mover la ficha " << ficha + 1 << ". Se mueve automáticamente de la casilla " << juego.jugadores[juego.jugadorTurno].fichas[ficha] << " a la casilla " << casilla << endl;
        mover(juego, ficha, casilla);
        ultimaFichaMovida = ficha;
    }
    else //Si no se puede mover ninguna ficha
        cout << "No puede mover ninguna de las fichas" << endl;
    if (((juego.tirada >= 6) && (juego.seises > 0)) || juego.premio > 0)
        pasarTurno = false;
    return pasarTurno;
}

void mostrar(const tJuego& juego) {
    int casilla, ficha;
    tColor jug;

    cout << "\x1b[2J\x1b[H"; // Se situa en la esquina superior izquierda
    setColor(Gris);
    cout << endl;

    // Filas con la numeraci�n de las casillas...
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << i / 10;
    cout << endl;
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << i % 10;
    cout << endl;

    // Borde superior...
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << '>';
    cout << endl;

    // Primera fila de posiciones de fichas...
    for (int i = 0; i < NUM_CASILLAS; i++) {
        setColor(juego.casillas[i].calle2);
        if (juego.casillas[i].calle2 != Ninguno)
            cout << primeraEn(juego.jugadores[juego.casillas[i].calle2], i) + 1;
        else
            cout << ' ';
        setColor(Gris);
    }
    cout << endl;

    // "Mediana"   
    for (int i = 0; i < NUM_CASILLAS; i++)
        if (esSeguro(i))
            cout << 'o';
        else
            cout << '-';
    cout << endl;


    // Segunda fila de posiciones de fichas...
    for (int i = 0; i < NUM_CASILLAS; i++) {
        setColor(juego.casillas[i].calle1);
        if (juego.casillas[i].calle1 != Ninguno)
            cout << segundaEn(juego.jugadores[juego.casillas[i].calle1], i) + 1;
        else
            cout << ' ';
        setColor(Gris);
    }
    cout << endl;

    jug = Amarillo;
    // Borde inferior...
    for (int i = 0; i < NUM_CASILLAS; i++)
        if (i == zanataJugador(jug)) {
            setColor(jug);
            cout << "V";
            setColor(Gris);
        }
        else if (i == salidaJugador(jug)) {
            setColor(jug);
            cout << "^";
            setColor(Gris);
            if (jug != Verde) jug = tColor(int(jug) + 1);
        }
        else
            cout << '>';
    cout << endl;

    // Metas y casas...
    for (int i = 0; i < NUM_FICHAS; i++) {
        casilla = 0;
        jug = Amarillo;
        setColor(jug);
        while (casilla < NUM_CASILLAS) {
            if (casilla == zanataJugador(jug)) {
                ficha = primeraEn(juego.jugadores[jug], 101 + i);
                if (ficha != -1) {
                    cout << ficha + 1;
                    if (cuantasEn(juego.jugadores[jug], 101 + i) > 1) {
                        ficha = segundaEn(juego.jugadores[jug], 101 + i);
                        if (ficha != -1) {
                            cout << ficha + 1;
                        }
                        else
                            cout << "V";
                    }
                    else
                        cout << "V";
                }
                else
                    cout << "VV";
                casilla++;
            }
            else if (casilla == salidaJugador(jug)) {
                if (juego.jugadores[jug].fichas[i] == -1) // En casa
                    cout << i + 1;
                else
                    cout << "^";
                if (jug != Verde) jug = tColor(int(jug) + 1);
                setColor(jug);
            }
            else
                cout << ' ';
            casilla++;
        }
        cout << endl;
    }

    // Resto de metas...
    for (int i = 105; i <= 107; i++) {
        casilla = 0;
        jug = Amarillo;
        setColor(jug);
        while (casilla < NUM_CASILLAS) {
            if (casilla == zanataJugador(jug)) {
                ficha = primeraEn(juego.jugadores[jug], i);
                if (ficha != -1) {
                    cout << ficha + 1;
                    if (cuantasEn(juego.jugadores[jug], i) > 1) {
                        ficha = segundaEn(juego.jugadores[jug], i);
                        if (ficha != -1) {
                            cout << ficha + 1;
                        }
                        else
                            cout << "V";
                    }
                    else
                        cout << "V";
                }
                else
                    cout << "VV";
                casilla++;
                if (jug != Verde) jug = tColor(int(jug) + 1);
                setColor(jug);
            }
            else
                cout << ' ';
            casilla++;
        }
        cout << endl;
    }

    casilla = 0;
    jug = Amarillo;
    setColor(jug);
    while (casilla < NUM_CASILLAS) {
        cout << ((juego.jugadores[jug].fichas[0] == 108) ? '1' : '.');
        cout << ((juego.jugadores[jug].fichas[1] == 108) ? '2' : '.');
        if (jug != Verde) jug = tColor(int(jug) + 1);
        setColor(jug);
        cout << "               ";
        casilla += 17;
    }
    cout << endl;
    casilla = 0;
    jug = Amarillo;
    setColor(jug);
    while (casilla < NUM_CASILLAS) {
        cout << ((juego.jugadores[jug].fichas[2] == 108) ? '3' : '.');
        cout << ((juego.jugadores[jug].fichas[3] == 108) ? '4' : '.');
        if (jug != Verde) jug = tColor(int(jug) + 1);
        setColor(jug);
        cout << "               ";
        casilla += 17;
    }
    cout << endl << endl;
    setColor(Gris);
}

void setColor(tColor color) {
    switch (color) {
    case Azul:
        cout << "\x1b[34;107m";
        break;
    case Verde:
        cout << "\x1b[32;107m";
        break;
    case Rojo:
        cout << "\x1b[31;107m";
        break;
    case Amarillo:
        cout << "\x1b[33;107m";
        break;
    case Gris:
    case Ninguno:
        cout << "\x1b[90;107m";
        break;
    }
}

void iniciaColores() {
#ifdef _WIN32
    for (DWORD stream : {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE}) {
        DWORD mode;
        HANDLE handle = GetStdHandle(stream);

        if (GetConsoleMode(handle, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(handle, mode);
        }
    }
#endif
}

void cargar(tJuego& juego, ifstream& archivo) {
    int jugador, casilla;
    for (int i = 0; i < NUM_JUGADORES; i++)
        for (int f = 0; f < NUM_FICHAS; f++) {
            archivo >> casilla;
            juego.jugadores[i].fichas[f] = casilla;
            if ((casilla >= 0) && (casilla < NUM_CASILLAS))
                if (juego.casillas[casilla].calle1 == Ninguno)
                    juego.casillas[casilla].calle1 = tColor(i);
                else {
                    if (casilla == salidaJugador(juego.jugadores[i].colorJugador)) { //Si ya hay una en la salida del jugador hay que moverla a la calle2 por si sale un 5
                        juego.casillas[casilla].calle2 = juego.casillas[casilla].calle1;
                        juego.casillas[casilla].calle1 = tColor(i);
                    }
                    else
                        juego.casillas[casilla].calle2 = tColor(i);
                }
        }
    archivo >> jugador;
    juego.jugadorTurno = tColor(jugador);
}

// FUNCIONES OPCIONALES 

int seleccionaJugHuman() {
    int jugSeleccionado = -1;
    while (jugSeleccionado < 0 || jugSeleccionado > 3) {
        setColor(Gris);
        cout << left << setw(22) << "Elige color:" << "Estilo de juego de la IA" << endl << endl;
        setColor(Amarillo);
        cout << left << setw(22) << "Amarillo = 0" << "Jugador agresivo, prioriza dañar a los enemigos " << endl << endl;
        setColor(Azul);
        cout << left << setw(22) << "Azul = 1" << "Jugador pasivo, prioriza la seguridad de sus fichas " << endl << endl;
        setColor(Rojo);
        cout << left << setw(22) << "Rojo = 2" << "Jugador agresivo, prioriza dañar a los enemigos " << endl << endl;
        setColor(Verde);
        cout << left << setw(22) << "Verde = 3" << "Jugador pasivo, prioriza la seguridad de sus fichas  " << endl << endl;
        setColor(Gris);
        cin >> jugSeleccionado;
        if (jugSeleccionado < 0 || jugSeleccionado > 3)
            cout << "Has introducido un número no valido, elige otro" << endl;
    }
    return jugSeleccionado;
}

int puedeComer(tJuego& juego, int& casilla) { //Devuelve la ficha de menor índice que se pueda comer a otra ficha (o -1 si ninguna puede)
    int ficha = -1, i = 0;
    while (ficha == -1 && i < NUM_FICHAS) {
        if (puedeMover(juego, i, casilla)) {
            if (casilla < NUM_CASILLAS) { //Si la casilla está en la calle
                if (!esSeguro(casilla)) {
                    if (juego.casillas[casilla].calle1 != Ninguno && juego.casillas[casilla].calle1 != juego.jugadorTurno)
                        ficha = i;
                }
            }
        }
        i++;
    }
    if (ficha != -1)
        cout << "MÁQUINA: Mi ficha " << ficha + 1 << " se puede comer a otra ficha!!" << endl;
    return ficha;
}

int aMeta(tJuego& juego, int& casilla) { //Devuelve la ficha de menor índice que pueda llegar a meta (o -1 si ninguna puede)
    int ficha = -1, i = 0;
    while (ficha == -1 && i < NUM_FICHAS) {
        if (puedeMover(juego, i, casilla)) {
            if (casilla == 108) //Si la casilla de llegada es la 108
                ficha = i;
        }
        i++;
    }
    if (ficha != -1)
        cout << "MÁQUINA: Vamos, mi ficha " << ficha + 1 << " llega a meta!!" << endl;
    return ficha;
}

int aSeguro(tJuego& juego, int& casilla) { //Devuelve la ficha de menor índice que pueda llegar a un seguro o a la subida a meta
    int ficha = -1, fichaAComprobar = -1, i = 0;
    while (ficha == -1 && i < NUM_FICHAS) {
        if (juego.jugadores[juego.jugadorTurno].fichas[i] < 100) {
            if (puedeMover(juego, i, casilla)) {
                if (casilla < NUM_CASILLAS && esSeguro(casilla) || casilla > 100) {
                    fichaAComprobar = i;
                    if (!esSeguro(juego.jugadores[juego.jugadorTurno].fichas[fichaAComprobar]))
                        ficha = fichaAComprobar;
                }
            }
        }
        i++;
    }
    if (ficha == -1)
        ficha = fichaAComprobar; //Si todas van de seguro a seguro se mueve la de mayor índice
    if (ficha != -1)
        cout << "MÁQUINA: Uy, mi ficha " << ficha + 1 << " puede llegar a una casilla segura :). Vamos a proteger la ficha " << ficha + 1 << endl;
    return ficha;
}

int huir(tJuego& juego, int& casilla) { //Devuelve la ficha que se encuentre en mayor peligro (o -1 si todas están a salvo)
    int ficha = -1, i = 0, casillaTrasera, distancia1 = 8, distancia2 = 8, llegada = 8; //Una distancia incial mayor a la posible para que distancia2 la primera vez sea necesariamente menor
    for (int i = 0; i < NUM_FICHAS; i++) {
        if ((juego.jugadores[juego.jugadorTurno].fichas[i] < 101) && (juego.jugadores[juego.jugadorTurno].fichas[i] != -1) && (!esSeguro(juego.jugadores[juego.jugadorTurno].fichas[i]))) { // Se comprueba si no estas en la subida de meta, en casa o en seguro
            if (!puente(juego, juego.jugadores[juego.jugadorTurno].fichas[i])) { //Si está en un puente no se considera que esté en peligro
                casillaTrasera = (juego.jugadores[juego.jugadorTurno].fichas[i] - 7 + NUM_CASILLAS) % NUM_CASILLAS; //Sumamos NUM_CASILLAS para que si el número es negativo se aplique el módulo (a partir de pruebas parecía que aplicar el módulo a un número negativo no producía ningún efecto)
                for (int j = 0; j < 7; j++) { //Se comprueban las 7 casillas anteriores a la de la ficha para ver si hay alguna ficha enemiga (es decir, la ficha está en peligro)
                    if (casillaTrasera == NUM_CASILLAS)
                        casillaTrasera = 0;
                    if (puedeMover(juego, i, casilla)) {
                        if (juego.casillas[casillaTrasera].calle1 != Ninguno) {
                            if (juego.casillas[casillaTrasera].calle1 != juego.jugadorTurno)
                                distancia2 = 7 - j;
                            else if (juego.casillas[casillaTrasera].calle2 != Ninguno && juego.casillas[casillaTrasera].calle2 != juego.jugadorTurno)
                                distancia2 = 7 - j;
                        }
                        if (distancia2 < distancia1) { //Comprobamos que distancia es mayor la que hemos calculado o la que hemos ido calculando antes
                            ficha = i;
                            distancia1 = distancia2;
                        }
                    }
                    casillaTrasera++;
                }
            }
        }
    }
    if (ficha != -1)
        cout << "MÁQUINA: Me conviene alejarme del peligro O_o. Voy a mover la ficha " << ficha + 1 << endl;
    return ficha;
}

int primeraPosible(tJuego& juego, int& casilla) { //Devuelve la ficha de menor índice que se pueda mover (preferiblemente sin irse de un seguro)
    int ficha = -1, fichaAComprobar = -1, i = 0;
    while (ficha == -1 && i < NUM_FICHAS) {
        if (puedeMover(juego, i, casilla)) {
            fichaAComprobar = i;
            if (casilla < NUM_CASILLAS && !esSeguro(juego.jugadores[juego.jugadorTurno].fichas[i]))
                ficha = fichaAComprobar;
        }
        i++;
    }
    if (ficha == -1) //Si todas están en seguros se moverá la última ficha capaz de hacerlo
        ficha = fichaAComprobar;
    cout << "MÁQUINA: Moveré la ficha " << ficha + 1 << endl;
    return ficha;
}