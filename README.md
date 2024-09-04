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
  - Sensor de nivel (Entrada Digital): Si esta encendido indica que el contenedor está lleno.
  - Sensor de gas (Entrada Analógica): Si supera un umbral indica que se detectó gas. Posible riesgo de incendio.
  - Detector De Gas (Entrada Analógica): Si supera un umbral indica que existe una temperatura elevada.
  - Detector De Humedad (Entrada Analógica): Sensa el nivel de humedad en el contenedor. Variable influyente en el riesgo de incendio. 
  - Sensor Tapa (Entrada Digital): Si esta encendido indica que la tapa esta cerrada.
  - Actuador Matafuegos (Salida Digital): Si esta en alto se abre el matafuegos.
  - Actuador Traba de la Tapa (Salida Digital): Con esta salida se puede bloquear la tapa.  
  - Actuador Tapa Ventilación (Salida Digital): Si está en alto se abre la tapa de ventilación.
  - Alarma (Salida Digital): Señal auditiva indicadora de posible incendio. 
  - UART: Comunicación con la computadora. Para enviar y recibir comandos. 
