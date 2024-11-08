package libroundrobin;

public class GestorProcesos {

    // Método nativo que llama al código C
    public native int[][] calculateProcessExecution(int cantidad, int[] prioridades, int[] tiemposLlegada, int[] duraciones);

}
