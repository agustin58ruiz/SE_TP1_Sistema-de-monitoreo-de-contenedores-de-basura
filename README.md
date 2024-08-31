# SiMBa (Sistama de Monitoreo de Basura)


## Alumno: Agustín Ruiz
## Descripción
La recolección de basura en la Ciudad de Buenos Aires es compleja y requiere de una inversión en recursos elevada. 

La recolección de residuos se realiza de forma agendada en donde los recolectores realizan una ruta definida. 

Debido a que los containers no siempre están cargados, o están cargados de más, se propone un sistema de monitoreo para llevar un registro en tiempo real del estado de los contenedores. 

El sistema se compone de dos partes. Por un lado, un sistema embebido capaz de realizar un sondeo de los parámetros del contenedor tales como: Capacidad, Humedad, Detección de gas, temperatura, presión, y también de accionar algunos mecanismos a implementar en el contendor como ventilación, bloqueo de la tapa, indicador de bateria. 

Por otro lado, se dispondrá de una central de datos que recepcionará todas las variables de los dispositivos que permitirá la implementación de rutas de recolección dinámicas, analisis de datos y utilización eficiente de los recursos. 

De esta forma se puede dar un mejor seguimiento al problema ambiental de la recolección de basura y una mejora en la eficacia de las políticas ambientales de la ciudad. 
## Plataforma de desarrollo:  NUCLEO-F429ZI
## Periféricos a utilizar: 
  - Botón1: Si esta apretado indica que el contenedor está lleno.
  - Botón2: Si esta encendido indica que se detectó gas. Posible riesgo de incendio.
  - Boton3: Si está encendido indica que existe una temperatura elevada.
  - Led1: Si esta encendido indica que la tapa esta cerrada.
  - Led2: Si esta encendido indica que se encendió el matafuegos
  - Led3: Si está encendido indica que se encendió la ventilación
  - UART: Comunicación con la computadora.
