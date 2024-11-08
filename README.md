## **Gestión de Procesos con Round-Robin usando JNI**

### *Creación paso a paso de Bibliotecas Dinámicas con JNI*

### Código en Java para la Integración JNI

Primero, crea la carpeta principal del proyecto, por ejemplo: `mkdir ova-libroundrobin-jni`. A continuación, crea una subcarpeta para el código en Java: `mkdir libroundrobin`. Dentro de esta subcarpeta, desarrolla el archivo `.java` con el comando `nano GestorProcesos.java`. El código en Java debería ser el siguiente:

```java
package libroundrobin;

public class GestorProcesos {

    // Método nativo que llama al código C
    public native int[][] calculateProcessExecution(int cantidad, int[] prioridades, int[] tiemposLlegada, int[] duraciones);

}
```

### Código en C

Después de finalizar el código en Java, genera el archivo `.h` utilizando: `javac GestorProcesos.java -h .`. Para verificar el contenido del archivo compilado, puedes usar el comando: `file GestorProcesos.class`.

Duplica el archivo de encabezado de la siguiente manera: `cp libroundrobin_GestorProcesos.h libroundrobin_GestorProcesos.c`, y luego edita el archivo `.c` usando `nano libroundrobin_GestorProcesos.c`. El código en C debería ser el siguiente:

```c
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

### Generación de la Biblioteca

Para compilar la biblioteca, utiliza el siguiente comando: `gcc -c -fPIE libroundrobin_GestorProcesos.c -I/usr/lib/jvm/java-1.17.0-openjdk-amd64/include/ -I/usr/lib/jvm/java-1.17.0-openjdk-amd64/include/linux/ -o libroundrobin.o`. Luego, genera la biblioteca compartida ejecutando: `gcc -shared libroundrobin.o -o libroundrobin.so`. Para finalizar la instalación de la biblioteca, ejecuta estos comandos:

- `sudo cp libroundrobin.so /usr/lib`
- `sudo cp libroundrobin_GestorProcesos.h /usr/include`
- `sudo ldconfig`

### Envío de Datos y Salida de Resultados en Java

Regresa a la carpeta principal `ova-libroundrobin-jni`, crea una nueva subcarpeta `mkdir gestorJNI`, y dentro de `gestorJNI`, crea el archivo `nano JavaJNIGestorApp.java` con el siguiente código:

```java
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

Al terminar el desarrollo del código, compílalo desde la carpeta principal con:

- `javac gestorJNI/JavaJNIGestorApp.java`
- `java gestorJNI.JavaJNIGestorApp`

### Resultado Final

Al ejecutar el programa, obtendrás una salida similar a:

```
El proceso 0 inició su ejecución en 1 y terminó de ejecutarse en 4
El proceso 1 inició su ejecución en 1 y terminó de ejecutarse en 3
El proceso 2 inició su ejecución en 2 y terminó de ejecutarse en 5
```
