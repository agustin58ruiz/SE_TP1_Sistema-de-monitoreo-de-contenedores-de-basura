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
  - Sensor de nivel (Entrada Digital) (D1): Si esta encendido indica que el contenedor está lleno.
  - Sensor de gas (Entrada Analógica) (A0): Si supera un umbral indica que se detectó gas. Posible riesgo de incendio.
  - Sensor de temperatura (Entrada Analógica) (A1): Si la temperatura aumenta a un nivel acciona la alarma.
  - Detector De Humedad (Entrada Analógica) (No implementado aún): Sensa el nivel de humedad en el contenedor. Variable influyente en el riesgo de incendio. 
  - Display (I2C): Indicador visual del estado del tacho. 
  - Sensor Tapa (Entrada Digital) (No implementado aún): Si esta encendido indica que la tapa esta cerrada.
  - Actuador Matafuegos (Salida Digital) (No implementado aún): Si esta en alto se abre el matafuegos.
  - Actuador Traba de la Tapa (Salida Digital) (LED1): Con esta salida se puede bloquear la tapa.  
  - Actuador Tapa Ventilación (Salida Digital) (No implementado Aun): Si está en alto se abre la tapa de ventilación.
  - Alarma (Salida Digital) (D2): Señal auditiva indicadora de posible incendio. 
  - UART: Comunicación con la computadora. Para enviar y recibir comandos. 

## Tercera entrega
En esta entrega se agregaron el sensor PIR, un motor a pasos que actuara sobre la tapa, manejo de interrupciones y inclución del ticker para tener un sistema menos bloqueante. 

Ahora la máquina de estados contiene los siguientes estados:  `TapaTrabada`, `TapaDestrabada`, `Inicio`, `PresenciaDeGas`, `AbriendoTapa`, `TapaAbierta`, `TapaCerrada` y `CerrandoTapa`

Se incluyó un sensor PIR para detectar la presencia de una persona y asi abrir la tapa automáticamente sin tener que tocar el contenedor. 

Se incluyó el manejo de interrupciones para realizar tareas como actualizar los estados de algunos sensores. Dentro de las interrupciones también se incluyeron varios `Ticker` para realizar cuentas regresivas o actualizar el estado de variables analógicas. 

También, a falta de un motor DC, se incluyó un motor a pasos que se utilizará para actuar sobre la tapa. Al motor a pasos se le asignó un Ticker para avanzar o retroceder un paso, de esta forma, el código del motor a pasos no es bloqueante. 

A continuación un enlace a un video con una demostración del funcionamiento del mismo. 

### Explicación de las conexiones y demostración
[![TP3](https://img.youtube.com/vi/PwA4jtDurbk/0.jpg)](https://www.youtube.com/watch?v=PwA4jtDurbk)

### Sobre la maquina de estados (maquinaDeEstados.cpp)
La maquina de estados se encuentra definida en el archivo maquinaDeEstados.cpp. Básicamente define tres clases: 
-  MaquinaDeEstados: Se encarga de gestionar la maquina de estados. Funciona solamente como un contenedor y tiene un puntero hacia el estado actual. Al inicializarse se debe colocar el primer estado de donde debe realizarse el procesamiento. Con procesamiento se entiende como la evaluación de si el estado debe avanzar hacia otro estado o continuar en el mismo. 
-  Transicion: Este objeto permite la transición entre estados, y es la representación de una arista dirigida si se considera a la máquina de estados como un grafo. Tiene una condición y una acción. La condición permite saber si la máquina debe transicionar a otro estado o no. De ser satisfactoria la condición, se realiza una acción cuyo proposíto puede ser accionar sobre variables del sistema. También se incorpora una Actualización, que es una transición cuya condición es siempre falsa y sirve para ejecutar actualizaciones en las variables del sistema. 
-  Estado:  Este objeto define un estado en el sistema y contiene un arreglo de transiciones que se utilizan para conectarlo con otros estados. El estado en esta implementación es simplemente un contenedor de las transiciones.
  
### Estados definidos
El sistema define los siguientes estados:

- INICIO: El estado inicial del sistema donde se realiza la configuración inicial y las actualizaciones.
- TAPA_TRABADA: Estado en el que la tapa está bloqueada.
- TAPA_DESTRABADA: Estado en el que la tapa está desbloqueada.
- PRESENCIA_DE_GAS: Estado en el que se ha detectado la presencia de gas.

A continuación, una imagen con el diagrama de estados que explica cualitativamente el funcionamiento del programa: 

![Diagrama de estados TP2 ](Images/TP2_DiagramaDeEstados.png "Cuatro estados: INICIO, TAPA_TRABADA, TAPA_DESTRABADA y PRESENCIA_DE_GAS.")
### Transiciones entre estados
Las transiciones determinan cómo moverse de un estado a otro en función de las condiciones detectadas por los sensores. Por ejemplo:

- Inicio a TAPA_DESTRABADA: Esta transición se produce si el sistema se inicializa correctamente.
- Inicio a PRESENCIA_DE_GAS: Si se detecta gas al inicio, el sistema cambia a este estado y activa la alarma.
- TAPA_DESTRABADA a TAPA_TRABADA: Cuando el contenedor está lleno y no hay presencia de gas, la tapa se bloquea.
- TAPA_TRABADA a TAPA_DESTRABADA: Se produce cuando hay espacio en el contenedor y no hay presencia de gas.
- PRESENCIA_DE_GAS a TAPA_TRABADA: Se activa si el contenedor está lleno pero no hay presencia de gas.
### Lógica de transiciones y acciones
Cada transición está definida con una condición y una acción:

- Condición: Una función que devuelve true o false dependiendo del estado de los sensores. Si devuelve true, se activa la transición.
- Acción: Una función que se ejecuta cuando se realiza la transición. Por ejemplo, activar una alarma, mostrar mensajes en el display, o bloquear la tapa.
### Funcionamiento
- Inicialización: Al iniciar el sistema, se configuran los estados y se realizan las actualizaciones iniciales. Esto incluye mostrar un mensaje en el display y actualizar el estado de los sensores.
- Evaluación del estado: El método Evaluar se ejecuta periódicamente para revisar el estado actual y determinar si debe cambiar a otro estado según las condiciones.

## Tercera entrega

En esta entrega se realizó la programación de la jerarquización de la maquina de estados que ahora incluye la posibilidad de insertar maquinas de estados dentro de un estado.
Se incluyo un sensor PIR para detectar la presencia de un usuario y realizar acciones en consecuencia como por ejemplo abrir la tapa, o encender el backlight del display.
Además, para apagar el backlight se utiliza un temporizador. La lógica consiste en que si el sensor PIR detecta a una persona, se enciende el backlight y se inicia un temporizador que apaga el backlight y quita el temporizador para que no vuelva a correr. 
Por último, se incluyó un motor a pasos cuya función el abrir o cerrar la tapa del tacho de basura. Para eso se hizo uso de interrupciones para controlar el motor. 

************************************
---> LINKS a videos
************************************

