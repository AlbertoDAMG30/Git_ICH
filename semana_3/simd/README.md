# Laboratorio 2: instrucciones SIMD con AVX2

Este laboratorio estudia el efecto de las instrucciones vectoriales sobre el rendimiento de una multiplicación de matrices de `2048 x 2048` elementos de tipo `float`. Se implementaron dos versiones del mismo algoritmo:

- `matmul_scalar.c`: versión de referencia, que procesa un elemento por iteración.
- `matmul_avx2.c`: versión SIMD que utiliza intrinsics de AVX2.

Ambas versiones transponen primero la segunda matriz para recorrer sus datos de manera contigua durante los productos punto. Esto mejora la localidad de memoria y permite que la comparación use la misma organización del algoritmo en los dos casos.

## Ejercicio D: comparación de rendimiento

### Fundamento teórico

El Capítulo 1, define el paralelismo a nivel de datos como el cómputo simultáneo de varios datos y relaciona este modelo con SIMD. En este laboratorio, los ocho carriles de un registro AVX2 explotan ese paralelismo dentro del producto punto.

El rendimiento es el inverso del tiempo de ejecución:


P = 1 / Te


Por ello, para una misma carga de trabajo, la versión que tarda menos posee mayor rendimiento. El *speedup* o aceleración se calcula como:


S = T_escalar / T_AVX2


La ley de Amdahl divide la ejecución en una fracción no mejorada `s` y una fracción paralelizable `p`, donde `p = 1 - s`:


S(n) = 1 / (s + p/n)


Para este análisis se usa `n = 8`, porque una operación AVX2 puede procesar ocho valores `float`. Este uso de Amdahl es un modelo aproximado, los carriles SIMD no son procesadores independientes y también existen costos de carga, transposición, control y reducción horizontal.

El Capítulo 2, refuerza que la aceleración queda limitada por las partes que no se benefician del paralelismo. También señala que el tiempo total incluye cómputo y movimiento o comunicación de datos. En este caso, AVX2 acelera principalmente las multiplicaciones y sumas del producto punto, pero no elimina el costo de transponer la matriz, recorrer los bucles, cargar datos desde memoria ni reducir los resultados vectoriales.

### Resultados experimentales

| Ensayo | Escalar (s) | AVX2 (s) |
|---:|---:|---:|
| 1 | 10.821325 | 2.868511 |
| 2 | 10.737388 | 3.084787 |
| 3 | 10.760614 | 2.916022 |
| **Promedio** | **10.773109** | **2.956440** |

El rendimiento promedio reportado por los programas fue:

| Versión | Rendimiento promedio |
|---|---:|
| Escalar | 1.594716 GFLOP/s |
| AVX2 | 5.816631 GFLOP/s |

Las dos implementaciones produjeron el mismo checksum:


86972906452.000000


Esta igualdad comprueba, para los datos utilizados, que la mejora de rendimiento no cambió el resultado calculado.

### Speedup y ley de Amdahl

Con los tiempos promedio:


S = 10.773109 / 2.956440
S = 3.643946


Por tanto, la versión AVX2 fue aproximadamente **3.64 veces más rápida** que la versión escalar en este equipo.

Para estimar la fracción no acelerada se despeja `s` de la ley de Amdahl usando `n = 8`:


s = (1/S - 1/n) / (1 - 1/n)
s = (1/3.643946 - 1/8) / (1 - 1/8)
s = 0.170775


La estimación resultante es:

- Fracción efectivamente vectorizable: `p = 82.92 %`.
- Fracción no acelerada: `s = 17.08 %`.
- Eficiencia respecto a ocho carriles: `S/8 = 45.55 %`.
- Límite estimado si la parte vectorizable pudiera acelerarse indefinidamente: `1/s = 5.86x`.

La aceleración observada no llega al máximo ideal de `8x` porque no todo el tiempo de ejecución corresponde a las multiplicaciones vectorizables. Además, el código realiza cargas de memoria, transposición, control de bucles y reducciones horizontales. De acuerdo con Amdahl, estos componentes no acelerados terminan limitando la ganancia total, aunque la sección principal de cómputo utilice ocho datos por instrucción.

## Conclusión

El uso explícito de AVX2 redujo el tiempo promedio de `10.773109 s` a `2.956440 s` y elevó el rendimiento de `1.594716` a `5.816631 GFLOP/s`. El speedup de `3.64x` confirma el beneficio del paralelismo SIMD para la multiplicación de matrices. Sin embargo, la ley de Amdahl explica por qué la mejora global es menor que los ocho valores procesados por instrucción: la parte no vectorizada y los costos asociados al movimiento y reducción de datos permanecen en la ejecución.