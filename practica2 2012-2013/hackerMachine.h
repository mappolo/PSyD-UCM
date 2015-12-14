#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

#define MAX_ROWS 10
#define MAX_COLS 10

#define MASTER 0

#define TRUE 1
#define FALSE 0

#define SAME 0
#define DIFFERENT 1


/*
 * Inicializa el sistema. Debe llamarse UNA vez antes de empezar a procesar la clave.
 *
 * @param rank ID del proceso que inicializa el sistema
 */
void initSystem(int rank);

/**
 * Esta función compara una fila de la matriz con la misma línea de la matriz que forma la clave.
 * El resultado de la ejecución es un bitMap. Cada posición indica si el valor es el mismo (0) o debe cambiarse (1)
 *
 * @param attemptMatrix Fila de la matriz que estamos procesando.
 * @param bitMap Array de salida que contiene información sobre la parte de la clave que es correcta y la que no lo es.
 * @param rowNumber Número de la fila que se está procesando.
 *
 */
void compareMatrixRow (int attemptMatrix[MAX_COLS], int bitMap[MAX_COLS], int rowNumber);

/**
 * Comprueba si una clave es la correcta o no lo es.
 *
 * @param attemptMatrix Matriz que vamos a comparar con la clave
 *
 * @return 1 si es la clave o 0 si no lo es
 *
 */
int checkCurrentKey (int attemptMatrix[MAX_ROWS][MAX_COLS]);

/**
 * Imprime una matriz por pantalla
 *
 * @matrix Matriz a imprimir
 */
void printMatrix (int matrix[MAX_ROWS][MAX_COLS]);
