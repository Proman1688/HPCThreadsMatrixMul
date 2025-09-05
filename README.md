# Multiplicación de Matrices con Pthreads

Este proyecto implementa la multiplicación de matrices cuadradas en C, utilizando programación paralela con **pthreads**.  
Permite comparar el tiempo de ejecución entre distintas configuraciones de número de hilos y dimensiones de la matriz.

---

## ⚙️ Requisitos

- **Sistema operativo**: Linux o WSL2 (Windows Subsystem for Linux).  
- **Compilador**: `gcc` con soporte para `pthread`.  
- **Memoria RAM suficiente**: para las pruebas grandes (matrices de 3200×3200 se requieren varios GB de RAM).  

---

## 🛠️ Compilación

Desde la terminal en la carpeta del proyecto:

```bash
gcc AppliedThreads.c -O2 -pthread -o matmul
```

El programa recibe dos argumentos:

```bash
./matmul <N> <num_threads>
```
Formato de la salida:
x.xxx ms
