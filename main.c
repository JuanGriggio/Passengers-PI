#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linesADT.h"

// Constante que dice el tamaño maximo de linea de los archivos .csv
#define MAX_LINE 100

// Funcion que lee el archivo de estaciones y las almacena en el TAD
void setLines (listADT lines, FILE* file) {
    // buffer es una variable auxiliar para ir leyendo el archivo.
    char * buffer = malloc(MAX_LINE * sizeof(char));

    // Obtengo el encabezado y no hago nada (porque no hace falta)
    fgets(buffer, MAX_LINE, file);

    // Proceso cada linea de estaciones.csv
    while (fgets(buffer, MAX_LINE, file) != NULL)  {
        // Guardo por linea los campos separados por comas en variables auxiliares
        char * id = strtok(buffer, ",");
        char * line = strtok(NULL, ",");
        char * name = strtok(NULL, "\n");

        // Me fijo si mi TAD ya tiene la linea. Si no la tiene, la inserto
        if(!elementBelongs(lines, line[0]))
            insertLine(lines, line[0]);

        // Inserto la nueva estacion
        insertStation(lines, line[0], name, id);
    }

    // Libero la memoria que le asigne a buffer porque ya termine de usarlo
    free(buffer);
}

// Funcion que lee el archivo de molinetes, procesa los datos obtenidos y almacena los resultados en el TAD.
// Esta funcion lee y obtiene los datos linea por linea, y los pasa a la funcion processData que es la que se encarga
// de almacenarlos en el TAD.
void setData (listADT lines, FILE* file) {
    // Idem setLines
    char * buffer = malloc(MAX_LINE * sizeof(char));

    // Idem setLines
    fgets(buffer, MAX_LINE, file);

    // Idem setLines
    while (fgets(buffer, MAX_LINE, file) != NULL)  {
        char * date = strtok(buffer, ",");
        char * from = strtok(NULL, ",");
        char * to = strtok(NULL, ",");
        char * stationId = strtok(NULL, ",");
        int passengers = atoi(strtok(NULL, "\n"));
        processData(lines, date, from, to, stationId, passengers);
    }

    // Idem setLines
    free(buffer);
}

int main(int argc, char *argv[]) {

    // Chequeo que la cantidad de argumentos sea correcta
    if(argc != 3) {
        printf("Numero de argumentos incorrecto\n");
        return -1;
    }

    // La lista donde voy a guardar los datos leidos de los archivos
    listADT lines = newList();

    // Abro el archivo con las estaciones, las almaceno en mi lista y cierro el archivo
    FILE* file_handler = fopen(argv[1], "r");
    setLines(lines, file_handler);
    fclose(file_handler);

    // Abro el archivo con los molinetes, proceso los datos, los almaceno en mi lista (esto lo hace la funcion setData)
    // y cierro el archivo
    file_handler = fopen(argv[2], "r");
    setData(lines, file_handler);
    fclose(file_handler);

    // Esta funcion muestra los datos por pantalla en vez de guardarlos en los archivos .csv. No es necesaria.
//    showLines(lines);

    // Generacion de los archivos .csv
    query1(lines);
    query2(lines);
    query3(lines);
    query4(lines);

    // Libero la memoria luego de generar las querys
    freeList(lines);
}
