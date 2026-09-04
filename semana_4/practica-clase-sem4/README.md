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

### Fundamento teórico

El Capítulo 1 establece que el rendimiento de un programa es el inverso de su tiempo de ejecución:


P = 1 / Te


Por lo tanto, para la misma cantidad de elementos e iteraciones, la versión con menor tiempo posee mayor rendimiento. El capítulo también explica que las instrucciones y los datos recorren una jerarquía de memoria y que los accesos más cercanos al procesador presentan menor latencia. Esto permite comprender que el rendimiento no depende únicamente de la operación aritmética, sino también del acceso al código y a los datos.

El Capítulo 2 señala que el tiempo total puede separarse en trabajo de cómputo y costos de comunicación o movimiento de información. En este ejercicio, ambas versiones realizan el mismo trabajo sobre los mismos vectores; la diferencia observada proviene de la forma en que el compilador y el enlazador organizan y llaman las funciones.

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