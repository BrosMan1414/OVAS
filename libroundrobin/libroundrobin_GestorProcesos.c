#include <jni.h>
#include <stdio.h>
#include "libroundrobin_GestorProcesos.h"

JNIEXPORT jobjectArray JNICALL Java_libroundrobin_GestorProcesos_calculateProcessExecution(JNIEnv *env, jobject obj, jint cantidad, jintArray prioridades, jintArray tiemposLlegada, jintArray duraciones) {
    // Convertir arrays Java a C
    jint *gestion_prioridades = (*env)->GetIntArrayElements(env, prioridades, 0);
    jint *gestion_tiemposLlegada = (*env)->GetIntArrayElements(env, tiemposLlegada, 0);
    jint *gestion_duraciones = (*env)->GetIntArrayElements(env, duraciones, 0);

    int roundrobin[cantidad][3];
    int resultado[cantidad][2];
    int tiempo_total = 0;

    // Inicialización y cálculo del tiempo total
    for (int i = 0; i < cantidad; i++) {
        roundrobin[i][0] = gestion_prioridades[i];
        roundrobin[i][1] = gestion_tiemposLlegada[i];
        roundrobin[i][2] = gestion_duraciones[i];
        resultado[i][0] = 0;
        resultado[i][1] = 0;
        tiempo_total += gestion_duraciones[i];
    }

    // Algoritmo de round-robin
    for (int t = 0; t <= tiempo_total; t++) {
        for (int i = 0; i < cantidad; i++) {
            if (t >= roundrobin[i][1] && roundrobin[i][2] > 0) {
                roundrobin[i][2]--;
                if (roundrobin[i][2] == 0) {
                    resultado[i][1] = t;
                }
            }
            if (gestion_duraciones[i] != roundrobin[i][2] && resultado[i][0] == 0) {
                resultado[i][0] = t;
            }
        }
    }

    // Creación de array 2D para devolver resultado
    jclass intArrCls = (*env)->FindClass(env, "[I");
    jobjectArray resultArray = (*env)->NewObjectArray(env, cantidad, intArrCls, NULL);
    for (int i = 0; i < cantidad; i++) {
        jint temp[2] = {resultado[i][0], resultado[i][1]};
        jintArray row = (*env)->NewIntArray(env, 2);
        (*env)->SetIntArrayRegion(env, row, 0, 2, temp);
        (*env)->SetObjectArrayElement(env, resultArray, i, row);
    }

    (*env)->ReleaseIntArrayElements(env, prioridades, gestion_prioridades, 0);
    (*env)->ReleaseIntArrayElements(env, tiemposLlegada, gestion_tiemposLlegada, 0);
    (*env)->ReleaseIntArrayElements(env, duraciones, gestion_duraciones, 0);

    return resultArray;
}
