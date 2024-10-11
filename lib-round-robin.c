#include <stdio.h>

int main(){

    int cantidad; // Cantidad de procesos
    printf("Cantidad de procesos que se van a ingresar:\n");
    scanf("%d", &cantidad);
    
    int tiempo_total = 0; // Tiempo total de ejecucion

    int gestion[cantidad][3];
    int roundrobin[cantidad][3];
    int resultado[cantidad][2] = {0};//Tiempo en que inicio y termino de ejecutarse el proceso

    for(int i=0; i < cantidad; i++){
        printf("Define la prioridad:\n");
        scanf("%d", &gestion[i][0]); // Prioridad
        roundrobin[i][0] = gestion[i][0];

        printf("Define el tiempo de llegada:\n");
        scanf("%d", &gestion[i][1]); // Tiempo de llegada o entrada
        roundrobin[i][1] = gestion[i][1];

        printf("Define el tiempo de duracion:\n");
        scanf("%d", &gestion[i][2]); // Tiempo de duracion o trabajo
        roundrobin[i][2] = gestion[i][2];

        tiempo_total += gestion[i][2];
    }

    for(int t=0; t <= tiempo_total; t++){
        for(int i=0; i < cantidad; i++){
            if(t >= roundrobin[i][1] && roundrobin[i][2] > 0){
                roundrobin[i][2]--; // Se va restando la duracion total del proceso, porque ya se ejecuto una vez
                if(roundrobin[i][2] == 0){
                    resultado[i][1] = t; // Donde t es el tiempo en el que el proceso termino
                }
            }
            if(gestion[i][2] != roundrobin[i][2] && resultado[i][0] == 0){
                resultado[i][0] = t; // Donde t es el tiempo en el que se ejecuto por primera vez
            }
        }
    }

    for(int i=0; i < cantidad; i++){
        printf("El proceso %d inicio su ejecucion en %d y termino de ejecutarse en %d\n", i, resultado[i][0], resultado[i][1]);
    }

    return 0;

}
