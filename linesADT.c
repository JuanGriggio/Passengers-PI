
// Estos defines fueron usados para que en pampero
#define _XOPEN_SOURCE 700
#define _USE_XOPEN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linesADT.h"
#include <time.h>

#define STATION_NAME_LENGHT 100
#define STATION_ID_LENGHT 5

// Son los nodos de la lista. Cada nodo es una linea
struct node {
    listElementT head;
    struct node * tail;

};

typedef struct node * nodeP;

// Es el encabezado de la lista
struct listCDT {
    long totalUsers;            // Usuarios totales. Campo usado para calcular el porcentaje de cada linea
    long totalDaytimeUsers[7];  // Usuarios diurnos por dia totales, vector [dom, lun, ..., vie, sab]
    long totalNightUsers[7];    // Usuarios nocturnos por dia totales, vector [dom, lun, ..., vie, sab]
    long totalUsersPerDay[7];   // Usuarios por dia totales, vector [dom, lun, ..., vie, sab]
    nodeP first;
};

// Estructura usada por la query1 para guardar en un vector, para cada linea sus usuarios, y poder reordenarlo
typedef struct usersPerLine {
    char name;
    long users;
} usersPerLine;

// Estructura usada por la query3 para guardar en un vector, para cada linea su porcentaje, y poder reordenarlo
typedef struct percentagesPerLine {
    char name;
    double percentage;
} percentagesPerLine;

// Por si ocurren errores
static void Error(const char* s) {
    fprintf(stderr, "%s", s);
    exit(EXIT_FAILURE);
}

//void showLines(listADT lines) {
//    printf("Total passengers: %ld\n", lines->totalUsers);
//    printf("Total daytime passengers: ");
//    for(int i = 0; i < 7; i++) {
//        printf("%ld  ", lines->totalDaytimeUsers[i]);
//    }
//    printf("\nTotal night passengers: ");
//    for(int i = 0; i < 7; i++) {
//        printf("%ld  ", lines->totalNightUsers[i]);
//    }
//    printf("\nTotal daily passengers: ");
//    for(int i = 0; i < 7; i++) {
//        printf("%ld  ", lines->totalUsersPerDay[i]);
//    }
//    printf("\nAverages:\n");
//    nodeP aux = lines->first;
//    while (aux != NULL) {
//        printf("Line %c: %g\n", aux->head.name, ((double) aux->head.totalLineUsers * 100) / lines->totalUsers);
//        aux = aux->tail;
//    }
//    printf("\n\n\n");
//
//
//
//    aux = lines->first;
//    while (aux != NULL) {
//        printf("Line %c: \n", aux->head.name);
//        printf("Total line users: %ld \n", aux->head.totalLineUsers);
//        printf("Most frequent station: %s\n", aux->head.mostFrequentStationName);
//        printf("Most frequent station users: %ld \n", aux->head.mostFrequentStationUsers);
//        printf("Stations: %d\n", aux->head.stationsNumber);
//        for (int i = 0; i < aux->head.stationsNumber; i++) {
//            printf("Station %d:\n", i + 1);
//            printf("Name: %s, id: %s, users: %ld\n", aux->head.stations[i].name, aux->head.stations[i].id, aux->head.stations[i].users);
//        }
//        aux = aux->tail;
//        printf("\n\n");
//    }
//}

// Funcion que dada una fecha nos dice si es domingo (0), lunes (1), ...
static int getWeekday(char * date) {
    int d = atoi(strtok(date, "/"));
    int m = atoi(strtok(NULL, "/"));
    int y = atoi(strtok(NULL, "/"));

    return (d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7;
}

// Funcion que dice, dada una hora inicial, si es de dia o de noche. No se hico mas compleja y no se necesitaron mas parametros (como
// minutos, hora final, minutos finales) porque el formato de las horas es uniforme, cada 15 minutos y con esta funcion se obtienen
// los resultados esperados
static int isDayOrNight(char * from) {
    int hourFrom = atoi(strtok(from, ":"));
    if (hourFrom >= 6 && hourFrom < 17)
        return 1;
    if (hourFrom < 6 || hourFrom >= 17)
        return 2;
    return -1;
}

// Funcion que dados los datos leidos de una linea del archivo molinetes.csv, los procesa y guarda los resultados en el TAD
void processData(listADT lines, char * date, char * from, char * to, char * stationId, int passengers) {

    // Nos fijamos a que dia de semana corresponde la entrada leida y si es diurna o nocturna, y lo guardamos en variables
    int dayOfWeek = getWeekday(date);
    int dayOrNight = isDayOrNight(from);

    // Agrego los pasajeros al numero de pasajeros totales y a los correspondientes segun su dia (recordar que totalUsersPerDay es un vector de
    // 7 posiciones que corresponden a los dias de la semana
    lines->totalUsers += passengers;
    lines->totalUsersPerDay[dayOfWeek] += passengers;

    // Me fijo si es de dia o de noche y sumo los pasajeros al dia y tiempo correspondientes
    switch (dayOrNight) {
        case 1: lines->totalDaytimeUsers[dayOfWeek] += passengers;
            break;
        case 2: lines->totalNightUsers[dayOfWeek] += passengers;
            break;
        default: Error("Not day or night!");
            break;
    }


    // Busco linea por linea a la estacion a la que le tengo que sumar los pasajeros
    nodeP auxLine = lines->first;
    station * auxStation = auxLine->head.stations;
    int foundStation = 0, i = 0;

    while (foundStation == 0) {
        if(strcmp(auxStation[i].id, stationId) == 0) {
            foundStation = 1;
        } else {
            i++;
        }
        if(i == auxLine->head.stationsNumber) {
            auxLine = auxLine->tail;
            auxStation = auxLine->head.stations;
            i = 0;
        }

    }

    // Una vez encontrada la estacion y la linea a la que pertenece, le sumo los pasajeros a la linea (para luego
    // calcular el porcentaje sobre el total) y a la estacion
    auxLine->head.totalLineUsers += passengers;
    auxStation[i].users += passengers;

    // Me fijo si luego de sumar los pasajeros a la estacion, esta pasa a ser la mas frecuente. Si lo es,
    // almaceno sus datos en el nodo de la linea
    if(auxStation[i].users > auxLine->head.mostFrequentStationUsers) {
        auxLine->head.mostFrequentStationUsers = auxStation[i].users;
        auxLine->head.mostFrequentStationName = realloc(auxLine->head.mostFrequentStationName, strlen(auxStation[i].name) + 1);
        strcpy(auxLine->head.mostFrequentStationName, auxStation[i].name);
    }
}

listADT newList(void) {
    return calloc(1, sizeof(struct listCDT));
}

// Funcion usada para comparar en  contains
static int compare(char elem1, char elem2 ) {
    return elem1 - elem2;
}

// Usada por elementBelongs
static int contains(nodeP first, char line) {
    int c;

    if(first == NULL || (c=compare(first->head.name, line)) > 0)
        return 0;

    if ( c == 0 )
        return 1;

    return contains( first->tail, line);
}

// Se fija si una linea es parte del TAD
int elementBelongs(listADT lines, char line) {
    return contains(lines->first, line);
}

// Funcion recursiva para insertar una nueva linea en orden
static nodeP insertRec(nodeP first, char name) {
    int c;

    // Me fijo si va en esta posicion o no (segun el orden que viene dado por la funcion compare)
    if( first == NULL || (c = compare(first->head.name, name)) > 0 )
    {
        // Genero un nuevo nodo (linea) y me fijo si habia memoria para generarlo
        nodeP aux = malloc(sizeof( struct node ));
        if (aux == NULL)
            Error("No hay lugar para otra linea\n");

        // Inicializo todos los campos de mi nodo, que es la nueva linea, con los valores correspondientes
        aux->tail = first;
        aux->head.name = name;
        aux->head.totalLineUsers = 0;
        aux->head.mostFrequentStationName = calloc(1, STATION_NAME_LENGHT * sizeof(char));
        aux->head.mostFrequentStationUsers = 0;
        aux->head.stations = malloc(sizeof(station));
        aux->head.stationsNumber = 0;
        return aux;
    }

    // Si la nueva linea no iba en este lugar, se llama a la funcion de manera recursiva a ver si va en el lugar siguiente de mi lista
    if( c < 0 )
        first->tail = insertRec( first->tail, name);
    return first;
}

// Inserto la nueva linea que no estaba en el TAD. Lo hago de manera recursiva para insertarlas ordenadas.
void insertLine(listADT lines, char name) {
    lines->first = insertRec(lines->first, name);
}

// Funcion que inserta una nueva estacion en el TAD.
// Notar que el TAD si o si va a tener la linea a la que pertenece la estacion, porque fue agregada previamente
void insertStation(listADT lines, char line, char * name, char * id) {
    // Creo un nodo auxiliar apuntando a la primera linea y me fijo si la linea de mi estacion es esa linea.
    // Si no lo es, paso a la siguiente linea hasta encontrarla.
    nodeP aux = lines->first;
    while(aux->head.name != line) {
        aux = aux->tail;
    }

    // Hago el realloc para que en el vector de estaciones dentro del nodo, haya lugar para una estacion mas.
    // Podria hacerlo de a bloques pero me parecio que gastaria un poco mas de memoria
    aux->head.stations = realloc(aux->head.stations, (aux->head.stationsNumber + 1) * sizeof(station));
    if (aux->head.stations == NULL)
        Error("No hay lugar para otra estacion\n");

    // En la posicion del vector de estaciones (dentro del nodo de la linea) que corresponde a la nueva estacion,
    // asigno memoria para el nombre y el id. Use esas constantes por si con otros archivos hay nombres mas largos o con
    // ids de hasta cinco digitos. Luego le asigno los valores correspondientes con srtcpy, pongo que pasaron cero usuarios
    // por esa estacion y aumento el numero de estaciones de esa linea.
    aux->head.stations[aux->head.stationsNumber].name = malloc(STATION_NAME_LENGHT * sizeof(char));
    aux->head.stations[aux->head.stationsNumber].id = malloc(STATION_ID_LENGHT * sizeof(char));
    strcpy(aux->head.stations[aux->head.stationsNumber].name, name);
    strcpy(aux->head.stations[aux->head.stationsNumber].id, id);
    aux->head.stations[aux->head.stationsNumber].users = 0;
    aux->head.stationsNumber++;
}

// Funcion que libera toda la memoria al finalizar el programa. Para cada nodo (linea), de cada posicion del vector
// de estaciones que tiene, libero la memoria ocupada por el nombre y el id, luego libero el vector de estaciones y
// el nombre de la estaicion mas frecuente, luego libero el nodo en si y por ultimo libero el encabezado de la lista.
void freeList(listADT lines) {
    nodeP nodeAux1 = lines->first;
    nodeP nodeAux2;

    while (nodeAux1 != NULL) {
        for(int i = 0; i < nodeAux1->head.stationsNumber; i++) {
            free(nodeAux1->head.stations[i].name);
            free(nodeAux1->head.stations[i].id);
        }
        free(nodeAux1->head.stations);
        free(nodeAux1->head.mostFrequentStationName);
        nodeAux2 = nodeAux1->tail;
        free(nodeAux1);
        nodeAux1 = nodeAux2;

    }
    free(lines);
}

// Funcion para comparar estructuras que contienen los numeros de usuarios de las lineas
static int compareUsersPerLine(usersPerLine * elem1, usersPerLine * elem2 ) {
    return elem1->users < elem2->users;
}

// Funcion para comparar estructuras que contienen los porcentajes de las lineas
static int comparePercentagesPerLine(percentagesPerLine * elem1, percentagesPerLine * elem2 ) {
    return elem1->percentage < elem2->percentage;
}

// Funcion generica del metodo burbuja para ordenar lo necesario para las queries
static void bubbleSort (void * v, size_t dim, size_t bytes, int (* cmp) (const void *, const void *)) {
    int i, j;
    void * aux = malloc(bytes);
    for (j = 0; j < dim - 1; j++) {
        for (i = 0; i < dim - j - 1; i++) {
            if (cmp((char *) v + i * bytes, (char *) v + (i + 1) * bytes) > 0) {
                memcpy(aux, (char *) v + i * bytes, bytes);
                memcpy((char *) v + i * bytes, (char *) v + (i + 1) * bytes, bytes);
                memcpy((char *) v + (i + 1) * bytes, aux, bytes);
            }
        }
    }
    free(aux);
}

// Primera query
void query1(listADT lines) {
    // Puntero a nodo auxiliar para recorrer las lineas
    nodeP aux = lines->first;

    // Vector auxiliar para guardar en cada posicion una estructura con el nombre de la linea y su cantidad de
    // pasajeros, y despues poder reordenarlo
    usersPerLine * auxArray = calloc(1, sizeof(usersPerLine));
    int dim = 0;

    // Voy almacenando en el vector auxiliar, para cada linea, su nombre y su cantidad de usuarios. El realloc es
    // para ir agrandando el vector para poder ir almacenando los datos de cada linea.
    while (aux != NULL) {
        auxArray = realloc(auxArray, (dim + 1) * sizeof(usersPerLine));
        auxArray[dim].name = aux->head.name;
        auxArray[dim].users = aux->head.totalLineUsers;
        dim++;
        aux = aux->tail;
    }

    // Genero el archivo query1.csv y le pongo el encabezado
    FILE * query1 = fopen("query1.csv", "w+");
    fprintf(query1, "Linea,Pasajeros\n");

    // Ordeno el vector que contiene estructuras con linea y usuarios segun la cantidad de usuarios descendente
    bubbleSort(auxArray, dim, sizeof(usersPerLine), (int (*) (const void *, const void *))compareUsersPerLine);

    // Para cada elemento del vector (es decir, cada linea), paso sus datos a query1.csv
    int i = 0;
    while (i < dim) {
        fprintf(query1, "%c,%ld\n", auxArray[i].name, auxArray[i].users);
        i++;
    }

    // Cierro el archivo
    fclose(query1);

    // Libero la memoria asignada para mi vector auxiliar porque ya no lo necesito mas
    free(auxArray);
}

// Segunda query
void query2(listADT lines) {
    // Vector con los dias de la semana usado para imprimir los valores en el archivo sin tener que repetir codigo
    char * days [] = {"domingo", "lunes", "martes", "miercoles", "jueves", "viernes", "sabado"};

    // Genero el archivo query2.csv y le pongo el encabezado
    FILE * query2 = fopen("query2.csv", "w+");
    fprintf(query2, "Dia,Diurnos,Nocturnos,Total\n");

    // Para cada dia de la semana, imprimo el dia y lo que corresponde (almacenado en el encabezado de la lista)
    for(int i = 0; i < 7; i++) {
        fprintf(query2, "%s,%ld,%ld,%ld\n", days[i], lines->totalDaytimeUsers[i], lines->totalNightUsers[i], lines->totalUsersPerDay[i]);
    }

    // Cierro el archivo
    fclose(query2);
}

// Tercera query
void query3(listADT lines) {
    // Puntero a nodo auxiliar para recorrer las lineas
    nodeP aux = lines->first;

    // Vector auxiliar para guardar en cada posicion una estructura con el nombre de la linea y su porcentaje,
    // y despues poder reordenarlo
    percentagesPerLine * auxArray = calloc(1, sizeof(percentagesPerLine));
    int dim = 0;

    // Voy almacenando en el vector auxiliar, para cada linea, su nombre y su porcentaje. El realloc es
    // para ir agrandando el vector para poder ir almacenando los datos de cada linea.
    while (aux != NULL) {
        auxArray = realloc(auxArray, (dim + 1) * sizeof(percentagesPerLine));
        auxArray[dim].name = aux->head.name;
        auxArray[dim].percentage = (double) (aux->head.totalLineUsers * 100) / lines->totalUsers;
        dim++;
        aux = aux->tail;
    }

    // Genero el archivo query3.csv y le pongo el encabezado
    FILE * query3 = fopen("query3.csv", "w+");
    fprintf(query3, "Linea,Porcentaje\n");

    // Ordeno el vector que contiene estructuras con linea y porcentaje segun el porcentaje
    bubbleSort(auxArray, dim, sizeof(percentagesPerLine), (int (*) (const void *, const void *))comparePercentagesPerLine);

    // Para cada elemento del vector (es decir, cada linea), paso sus datos a query3.csv
    int i = 0;
    while (i < dim) {
        fprintf(query3, "%c,%g\n", auxArray[i].name, auxArray[i].percentage);
        i++;
    }

    // Cierro el archivo
    fclose(query3);

    // Libero la memoria asignada para mi vector auxiliar porque ya no lo necesito mas
    free(auxArray);

}

// Cuarta query
void query4(listADT lines) {
    // Puntero a nodo auxiliar para recorrer cada linea
    nodeP aux = lines->first;

    // Genero el archivo query4.csv y le pongo el encabezado
    FILE * query4 = fopen("query4.csv", "w+");
    fprintf(query4, "Estacion,Linea,Pasajeros\n");

    // Para cada linea, imprimo en el archivo su nombre, el nombre de la estacion mas frecuente y su cantidad de usuarios
    while (aux != NULL) {
        fprintf(query4, "%c,%s,%ld\n", aux->head.name, aux->head.mostFrequentStationName, aux->head.mostFrequentStationUsers);
        aux = aux->tail;
    }

    // Cierro el archivo
    fclose(query4);
}
