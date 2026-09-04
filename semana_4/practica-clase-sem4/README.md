## Ejercicio A: biblioteca estática

La biblioteca estática `libvectorops.a` contiene las operaciones de llenado y suma de vectores. Durante el enlazado, el código requerido se incorpora directamente en `bench-static`, por lo que el ejecutable no necesita `libvectorops.a` para ejecutarse posteriormente.

### Resultados experimentales

| Ensayo | Fill A (μs/iteración) | Fill B (μs/iteración) | Suma (μs/iteración) | Total (s) |
|---:|---:|---:|---:|---:|
| 1 | 858.487 | 852.152 | 1679.773 | 3.390413 |
| 2 | 832.056 | 814.047 | 1698.351 | 3.344455 |
| 3 | 899.944 | 844.366 | 1705.904 | 3.450214 |
| **Promedio** | **863.496** | **836.855** | **1694.676** | **3.395027** |

El archivo `libvectorops.a` generado tiene un tamaño exacto de `1832 bytes`, mostrado como `1.8K` por `ls -lh`.

La suma tarda aproximadamente el doble que cada llenado debido a que debe leer los dos vectores de entrada y escribir el vector de salida. En cambio, cada operación de llenado escribe solamente un vector.

## Ejercicio B: biblioteca dinámica

La versión dinámica utiliza `libvectorops.so`. A diferencia de la versión estática, el código de las operaciones vectoriales no se copia dentro del ejecutable: el cargador dinámico debe localizar la biblioteca y resolver sus símbolos cuando se ejecuta `bench-dynamic`.

### Resultados experimentales

| Ensayo | Fill A (μs/iteración) | Fill B (μs/iteración) | Suma (μs/iteración) | Total (s) |
|---:|---:|---:|---:|---:|
| 1 | 1994.002 | 1943.064 | 2023.702 | 5.960769 |
| 2 | 1967.733 | 1946.278 | 2009.370 | 5.923382 |
| 3 | 1961.040 | 1934.935 | 2020.626 | 5.916602 |
| **Promedio** | **1974.259** | **1941.425** | **2017.899** | **5.933584** |

El archivo `libvectorops.so` generado tiene un tamaño exacto de `15384 bytes`, mostrado como `16K` por `ls -lh`.

La verificación con `ldd` confirma que `bench-dynamic` depende del archivo externo:


`libvectorops.so => build/bin/../lib/libvectorops.so`


El `rpath` configurado en el `Makefile` permite encontrar la biblioteca dentro de `build/lib`. Si `libvectorops.so` se elimina o se mueve sin actualizar la ruta de búsqueda, `bench-dynamic` no puede iniciar.

### Comparación con la versión estática

| Operación | Estática (μs/iteración) | Dinámica (μs/iteración) | Relación dinámica/estática |
|---|---:|---:|---:|
| Fill A | 863.496 | 1974.259 | 2.29x |
| Fill B | 836.855 | 1941.425 | 2.32x |
| Suma | 1694.676 | 2017.899 | 1.19x |
| **Tiempo total** | **3.395027 s** | **5.933584 s** | **1.75x** |

En estas mediciones, la versión dinámica tardó un `74.77 %` más que la estática. De forma equivalente, la versión estática completó la carga en aproximadamente un `42.78 %` menos de tiempo.

La diferencia no se debe solamente a cargar la biblioteca al iniciar el programa. La inspección del código generado muestra una diferencia dentro de los ciclos principales:

- En la versión estática, GCC integra `value_from_index` dentro de `fill_vector` y `add_values` dentro de `add_vectors`, eliminando una llamada por elemento.
- En la biblioteca dinámica, `fill_vector` ejecuta una llamada a `value_from_index@plt` y `add_vectors` una llamada a `add_values@plt` por cada elemento.

La PLT permite que los símbolos de una biblioteca compartida sean resueltos o reemplazados durante la carga. En esta compilación, esa posibilidad impide que GCC aplique las mismas integraciones entre las funciones públicas de la biblioteca. Como se procesan mil millones de elementos durante los `1000` ensayos internos, el costo de una llamada adicional por elemento se acumula y se vuelve visible.

Aunque `libvectorops.so` ocupa más bytes que `libvectorops.a`, estos archivos tienen formatos y propósitos diferentes. El archivo `.a` es un contenedor de objetos para el enlazador, mientras que el `.so` es un objeto ELF cargable que necesita tablas de símbolos, información de reubicación y metadatos para el enlazado dinámico. Por ello, su tamaño no debe compararse como si almacenaran exactamente la misma estructura.

## Ejercicio C: funciones `static inline`

Las funciones de esta versión están definidas en `vector_ops_inline.h`. Al incluirse en la misma unidad de compilación que `main`, GCC puede analizar su contenido e insertar las operaciones directamente en los ciclos del programa.

### Resultados experimentales

| Ensayo | Fill A (μs/iteración) | Fill B (μs/iteración) | Suma (μs/iteración) | Total (s) |
|---:|---:|---:|---:|---:|
| 1 | 876.797 | 896.529 | 1692.438 | 3.465764 |
| 2 | 842.909 | 854.104 | 1733.870 | 3.430883 |
| 3 | 851.609 | 843.847 | 1740.054 | 3.435511 |
| **Promedio** | **857.105** | **864.826** | **1722.121** | **3.444053** |

### Comparación

| Versión | Tiempo promedio (s) | Diferencia frente a inline |
|---|---:|---:|
| Estática | 3.395027 | -1.44 % |
| Dinámica | 5.933584 | +72.28 % |
| **Static inline** | **3.444053** | **Referencia** |

La versión inline obtuvo un tiempo similar al de la biblioteca estática y fue un `41.96 %` más rápida que la dinámica. La diferencia de `1.44 %` respecto a la estática es pequeña y puede atribuirse a la variación normal de las mediciones y al costo dominante de recorrer los vectores en memoria.

La inspección del ejecutable confirma que no existen símbolos ni llamadas a `fill_vector_inline`, `add_vectors_inline`, `value_from_index_inline` o `add_values_inline`. GCC insertó estas operaciones dentro de `main`. Esto le permite optimizar a través de los límites de las funciones, mientras que una biblioteca compilada por separado limita la información disponible durante la compilación del programa.

De acuerdo con la definición de rendimiento del Capítulo 1, los tiempos cercanos de las versiones estática e inline representan un rendimiento equivalente para esta prueba. Declarar una función `static inline` facilita la optimización, pero no garantiza una mejora visible cuando el acceso a memoria domina la carga de trabajo.

## Ejercicio D: tamaño de los archivos

| Archivo | Tamaño en disco | Secciones (`text + data + bss`) |
|---|---:|---:|
| `bench-static` | 16888 B | 5785 B |
| `bench-dynamic` | 16776 B | 5722 B |
| `bench-inline` | 16704 B | 5911 B |
| `bench-static-lto` | 16488 B | 5062 B |
| `libvectorops.a` | 1832 B | 321 B |
| `libvectorops.so` | 15384 B | 2110 B |

`bench-static` es el ejecutable más grande en disco porque incorpora las funciones de `libvectorops.a`. `bench-dynamic` es menor porque conserva esas funciones en `libvectorops.so`, archivo externo requerido durante la ejecución. Esto fue comprobado con `ldd`.

`libvectorops.so` es mayor que `libvectorops.a` porque contiene, además del código, encabezados ELF, tablas de símbolos y datos de reubicación necesarios para la carga dinámica. `bench-static-lto` es el menor ejecutable porque LTO elimina o integra código durante el enlace.

El tamaño en disco y la suma mostrada por `size` miden aspectos distintos. El primero incluye todo el archivo y su alineamiento; el segundo muestra las secciones principales que forman el programa.

## Ejercicio E: comparación con LTO

Se ejecutó `run-all` tres veces con los mismos argumentos. Los siguientes valores son los promedios:

| Versión | Fill A (μs/iteración) | Fill B (μs/iteración) | Suma (μs/iteración) | Total (s) |
|---|---:|---:|---:|---:|
| Estática | 868.495 | 862.863 | 1728.209 | 3.459567 |
| Dinámica | 1956.511 | 1957.520 | 2028.347 | 5.942379 |
| `static inline` | 844.114 | 826.874 | 1722.512 | 3.393500 |
| Estática con LTO | 844.688 | 840.480 | 1720.680 | 3.405849 |

LTO fue solo un `0.36 %` más lento que inline, diferencia que no es significativa para estas mediciones. El ejecutable LTO tampoco conserva símbolos separados para las operaciones vectoriales, lo que confirma que `-flto` permitió optimizar entre `benchmark_library.c` y `vector_ops.c` durante el enlace.

La versión dinámica continuó siendo la más lenta porque mantiene llamadas mediante la PLT dentro de los ciclos. Las otras tres versiones producen ciclos similares y quedan principalmente limitadas por el recorrido de los vectores en memoria.

## Diferencias entre las implementaciones

- **Biblioteca estática:** copia en el ejecutable el código utilizado de la biblioteca. No necesita el archivo `.a` para ejecutarse, pero puede aumentar el tamaño del binario.
- **Biblioteca dinámica:** mantiene el código en un archivo `.so` que puede compartirse y actualizarse de forma independiente. El ejecutable depende de ese archivo y sus símbolos se resuelven durante la carga.
- **`static inline`:** coloca la definición en el encabezado y permite que el compilador inserte el código en el punto de uso. Facilita optimizaciones, aunque `inline` no garantiza por sí solo una mejora de rendimiento.
- **LTO:** conserva los archivos fuente separados, pero permite analizarlos juntos durante el enlace. En esta prueba logró un resultado equivalente a `static inline`.

## Fundamento teórico

El Capítulo 1 define el rendimiento como el inverso del tiempo de ejecución:


P = 1 / Te


Por ello, las versiones se compararon con la misma cantidad de elementos e iteraciones. La versión con menor tiempo presenta mayor rendimiento.

La jerarquía de memoria y la localidad espacial también influyen. Los ciclos recorren posiciones contiguas, pero los tres vectores ocupan cerca de `24 MB`, más que la caché L3 de `6 MiB` del equipo. Por esta razón, el movimiento de datos limita a las versiones estática, inline y LTO, aunque se eliminen llamadas a funciones.

El Capítulo 2 separa el tiempo de un proceso en cómputo y comunicación o movimiento de datos. En este experimento, todas las versiones realizan las mismas operaciones; las diferencias proceden de las llamadas generadas, la visibilidad disponible para el compilador y el acceso a memoria. La versión dinámica conserva llamadas mediante la PLT, mientras que inline y LTO permiten optimizar entre funciones o archivos.

## Conclusión

La versión dinámica ofrece reutilización y actualización independiente, pero fue la más lenta y requiere `libvectorops.so`. Las versiones estática, inline y LTO obtuvieron tiempos cercanos. LTO igualó el rendimiento de inline y produjo el ejecutable más pequeño.
