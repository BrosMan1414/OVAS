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
