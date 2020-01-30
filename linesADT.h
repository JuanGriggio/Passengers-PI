#ifndef LINESADT_H
#define LINESADT_H

typedef struct station {
    char * name;        // NOMBRE DE LA ESTACION
    long users;         // CANTIDAD DE USUARIOS DE LA ESTACION
    char * id;          // ID DE LA ESTACION
} station;

typedef struct listElementT {
    char name;
    long totalLineUsers;            // USUARIOS TOTALES DE LA LINEA
    long mostFrequentStationUsers;  // USUARIOS DE LA ESTACION CON MAS USUARIOS
    char * mostFrequentStationName; // NOMBRE DE LA ESTACION CON MAS USUARIOS
    station * stations;             // PUNTERO A STRUCT PARA QUE PUEDAN HABER VARIAS ESTACIONES Y SE PUEDAN IR AGREGANDO
    int stationsNumber;
} listElementT;

typedef struct listCDT *listADT;

/* Negativo: el primero antes del segundo
 * Cero: iguales
 * Positivo: el primero despues del segundo */

listADT newList(void);

void insertLine(listADT  list, char name);

//void showLines(const listADT list);

void processData(listADT list, char * date, char * from, char * to, char * stationId, int passengers);

void insertStation( listADT list, char line, char * name, char * id);

int elementBelongs(const listADT list, char line);

void freeList(listADT list);

void query1(listADT lines);

void query2(listADT lines);

void query3(listADT lines);

void query4(listADT lines);


#endif
