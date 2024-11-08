## **Gestion de procesos Round-Robin**


### *Creacion paso a paso de Bibliotecas Dinamicas con JNI*

### Codigo en Java para la integracion JNI:

Primero que todo se creara la carpeta principal del proyecto, ejemplo `mkdir ova-libroundrobin-jni`. Luego se crea una subcarpeta para el desarrollo del codigo en java, ejemplo `mkdir libroundrobin`. Se crea el archivo `.java` y se desarrolla el codigo, ejemplo `nano GestorProcesos.java`. Codigo de java:

```
package libroundrobin;

public class GestorProcesos {

    // Método nativo que llama al código C
    public native int[][] calculateProcessExecution(int cantidad, int[] prioridades, int[] tiemposLlegada, int[] duraciones);

}
```



### Codigo en C:

Luego de terminar nuestro codigo en java generaremos un archivo `.h` de esta forma `javac GestorProcesos.java -h .`. Para revisar lo que tiene el archivo compilado se puede usar: `file GestorProcesos.class`.

Ahora duplicaremos el archivo de cabezera, asi: `cp libroundrobin_GestorProcesos.h libroundrobin_GestorProcesos.c` y luego editamos el archivo `.c`, `nano libroundrobin_GestorProcesos.c`. Nuestro codigo desarrollado en c:


```
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
```



### Generar biblioteca:

El comando para crear el compilador de la biblioteca `gcc -c -fPIE libroundrobin_GestorProcesos.c -I/usr/lib/jvm/java-1.17.0-openjdk-amd64/include/ -I/usr/lib/jvm/java-1.17.0-openjdk-amd64/include/linux/ -o libroundrobin.o`. Luego generaremos la biblioteca con el comando: `gcc -shared libroundrobin.o -o libroundrobin.so`. Para terminar de instalar la biblioteca se tienen que engresar esots otros tres comandos:

- `sudo cp libroundrobin.so /usr/lib`
- `sudo cp libroundrobin_GestorProcesos.h /usr/include`
- `sudo ldconfig`



### Envio de los datos y salida de resultados en java:

Simplemente volvemos a nuestra carpeta principal `ova-libroundrobin-jni` y creamos una nueva carpeta `mkdir gestorJNI`, ya dentro de `gestorJNI` creamos el archivo `nano JavaJNIGestorApp.java`, aqui desorrallaremos nuestro codigo:

```
package gestorJNI;

import libroundrobin.GestorProcesos;

public class JavaJNIGestorApp {
    GestorProcesos scheduler;

    static {
        System.loadLibrary("roundrobin");
    }

    public static void main(String[] args) {
        new JavaJNIGestorApp();
    }


    public JavaJNIGestorApp() {
        GestorProcesos scheduler = new GestorProcesos();

        // Simulación de entrada de datos
        int cantidad = 3;
        int[] prioridades = {1, 2, 3};
        int[] tiemposLlegada = {0, 1, 2};
        int[] duraciones = {5, 3, 4};

        // Llamada al método nativo
        int[][] resultado = scheduler.calculateProcessExecution(cantidad, prioridades, tiemposLlegada, duraciones);

        // Imprimir el resultado
        for (int i = 0; i < resultado.length; i++) {
            System.out.printf("El proceso %d inició su ejecución en %d y terminó de ejecutarse en %d\n", i, resultado[i][0], resultado[i][1]);
        }
    }
}
```

Al terminar el desarrollo del codigo solo quedaria volver a la carpeta principal y compilar el ultimo codigo realizado: 

- `javac gestorJNI/JavaJNIGestorApp.java` 
- `java gestorJNI.JavaJNIGestorApp`


Mi resultado final:

```
El proceso 0 inició su ejecución en 1 y terminó de ejecutarse en 4
El proceso 1 inició su ejecución en 1 y terminó de ejecutarse en 3
El proceso 2 inició su ejecución en 2 y terminó de ejecutarse en 5
```
