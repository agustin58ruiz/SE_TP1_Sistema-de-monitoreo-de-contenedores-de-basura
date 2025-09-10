# SiMBa (Sistama de Monitoreo de Basura)

## Alumno: Agustín Ruiz
## Fecha: 2. º cuatrimestre 2024

# Memoria del Trabajo Final: SiMBa (Sistama de Monitoreo de Basura)


**Universidad de Buenos Aires**
**Facultad de Ingeniería**
**86.65 Sistemas Embebidos**

**Autor:** Agustín Ezequiel Ruiz

**Padrón:** 99124

**Profesores:** 
Ariel Lutenberg, Jorge Graña y Juan Cruz

*Este trabajo fue realizado en la Ciudad de Buenos Aires entre Agosto 2024 y Agosto de 2025.*

## Resumen
Este proyecto se trata de un sistema embebido para monitorear contenedores de basura para una recoleccion eficiente de los reciduos implementado con la placa de desarrollo STM32 Nucleo-F429ZI y la placa ESP-01. 
El sistema monitorea variables como temperatura, estado de la tapa, y presencia de gases inflamables y actua automaticamente siguiendo una maquina de estados. 
El dispositivo incluye conectividad mediante WiFi, lo que permite un control remoto y la posibilidad de realizar un monitoreo centralizado asi como tambien su control. 

Este trabajo tiene como fundamento que los recursos son escasos y debe hacerce un uso eficiente de los mismos. Los camiones de basura pueden reducir las emisiones de gases de efecto invernadero si siguen rutas optimizadas. 


## Tabla de Contenidos

- [1. Introducción](#1-introducción-general)
- [2. Introducción Específica](#capítulo-2-Introducción-específica)
- [3. Diseño e Implementación](#capítulo-3-Diseño-e-implementación)
- [4. Ensayos y resultados](#capítulo-4-Ensayos-y-resultados)
- [5. Conclusiones](#capı́tulo-5-conclusiones)

## Registro de versiones

| **Revisión** | **Cambios realizados** |  **Fecha** |
|:------------:|:----------------------:|:----------:|
|       1      | Creación del documento | 11/07/2025 |
|       2      |                        |            |
|       3      |                        |            |

## 1. Introducción general

### 1.1. Objetivo
Desarrollar un sistema embebido para optimizar la gestión y recolección de residuos a nivel municipal, reduciendo la emisión de gases de efecto invernadero y contribuyendo al cuidado del medioambiente.

### 1.2. Introducción
La recolección y el tratamiento de los residuos es un problema serio, ya que impacta de manera directa en la calidad de vida. La recolección de la basura puede mejorarse si, en lugar de seguir rutas estáticas, se implementaran rutas dinámicas generadas mediante algoritmos de grafos que permitan obtener un recorrido de recolección óptimo. Para que esto sea posible, es necesario conocer el estado de los contenedores de basura. El primer desafío a resolver es encontrar la forma de recopilar el estado de los contenedores y centralizar esos datos para la posterior generación de una ruta.

El presente proyecto aborda esta problemática mediante el desarrollo de un sistema embebido que permite monitorear variables como capacidad, temperatura y presencia de gases inflamables, además de enviar dichas variables a un concentrador de datos que calcule una ruta de recolección óptima. Como agregado, también se propusieron controles adicionales sobre la tapa que, si bien no son necesarios para el recolector, sí lo son para el usuario.

Este proyecto abarca distintos tópicos de la materia Sistemas Embebidos, tales como máquinas de estado, puertos de uso general, entradas analógicas, comunicación UART, comandos AT y conectividad WiFi mediante el módulo ESP-01.

Con esta propuesta se busca mejorar el sistema de recolección de basura y, de este modo, optimizar los recursos.


### 1.2. Análisis de sistemas similares en el mercado

Se analizaron cinco contenedores de basura. Se muestra la comparación de características en la Tabla 1.

<table style="height: 359px; width: 1313px;" border="1" cellspacing="0" cellpadding="5">
<thead>
<tr style="height: 52px;">
<th style="height: 52px; width: 85px;">Caracter&iacute;stica</th>
<th style="height: 52px; width: 268px;">[Tacho inteligente de 10L](https://www.mercadolibre.com.ar/tacho-cesto-de-basura-inteligente-10-litros-con-sensor-color-blanco/p/MLA53137335)</th>
<th style="height: 52px; width: 200px;">[Cesto de basura inteligente](https://articulo.mercadolibre.com.ar/MLA-1508280537-tacho-cesto-de-residuos-blanco-apertura-automatica-de-15-lts-_JM)</th>
<th style="height: 52px; width: 200px;">[Contenedor Bigbelly Smart](<a href="https://bigbelly.com/products/bigbelly-smart">https://bigbelly.com/products/bigbelly-smart</a>)</th>
<th style="height: 52px; width: 245.469px;">[Contenedor bilateral - OMBU](https://ombuhigieneurbana.com.ar/contenedor-carga-bilateral.php)</th>
<th style="height: 52px; width: 289.531px;">[Contenedor Carga Trasera - OMBU](https://ombuhigieneurbana.com.ar/contenedor-carga-trasera.php)</th>
</tr>
</thead>
<tbody>
<tr style="height: 1px;">
<td style="height: 1px; width: 85px;">Capacidad</td>
<td style="height: 1px; width: 268px;">10 Litros</td>
<td style="height: 1px; width: 200px;">15 Litros</td>
<td style="height: 1px; width: 200px;">178 Litros</td>
<td style="height: 1px; width: 245.469px;">40/90/150 Litros</td>
<td style="height: 1px; width: 289.531px;">800/1200L</td>
</tr>
<tr style="height: 39px;">
<td style="height: 39px; width: 85px;">Conectividad</td>
<td style="height: 39px; width: 268px;">No posee.</td>
<td style="height: 39px; width: 200px;">No posee.</td>
<td style="height: 39px; width: 200px;">LoraWAN - WIFI</td>
<td style="height: 39px; width: 245.469px;">No Posee</td>
<td style="height: 39px; width: 289.531px;">No posee.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px; width: 85px;">Apertura</td>
<td style="height: 13px; width: 268px;">Con sensor, sin contacto.</td>
<td style="height: 13px; width: 200px;">Con sensor, sin contacto.</td>
<td style="height: 13px; width: 200px;">Con pie. Tapa Lateral</td>
<td style="height: 13px; width: 245.469px;">Dos tapas laterales. Descarga de residuos inferior.</td>
<td style="height: 13px; width: 289.531px;">Una tapa superior.&nbsp;</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px; width: 85px;">Fuente de alimentaci&oacute;n</td>
<td style="height: 13px; width: 268px;">4 Pilas AA</td>
<td style="height: 13px; width: 200px;">2 Pilas AA</td>
<td style="height: 13px; width: 200px;">Panel Solar y bater&iacute;a recargable.&nbsp;</td>
<td style="height: 13px; width: 245.469px;">No posee.</td>
<td style="height: 13px; width: 289.531px;">No posee.</td>
</tr>
<tr style="height: 26px;">
<td style="height: 26px; width: 85px;">Usos</td>
<td style="height: 26px; width: 268px;">Privado, adentro de una casa.</td>
<td style="height: 26px; width: 200px;">Privado, adentro de una casa.</td>
<td style="height: 26px; width: 200px;">P&uacute;blico. Sistema de recolecci&oacute;n de residuos. Peque&ntilde;os reciduos (latas, bolsas de comida, m&aacute;x 5 Litros) por el tama&ntilde;o de la tapa.&nbsp;</td>
<td style="height: 26px; width: 245.469px;">P&uacute;blico-urbano. Sistema de recolecci&oacute;n de residuos.</td>
<td style="height: 26px; width: 289.531px;">P&uacute;blico-urbano. Sistema de recolecci&oacute;n de residuos.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px; width: 85px;">Caracter&iacute;sticas Extras</td>
<td style="height: 13px; width: 268px;">Pl&aacute;stico, posee led indicador. tapa extra&iacute;ble.</td>
<td style="height: 13px; width: 200px;">Pl&aacute;stico, posee led indicador. tapa extra&iacute;ble.</td>
<td style="height: 13px; width: 200px;">Antiband&aacute;lico, materiales reforzados. Sistema de monitoreo del estado del contenedor en tiempo real.&nbsp;</td>
<td style="height: 13px; width: 245.469px;">Reforzado, met&aacute;lico, adaptable con sistema antiband&aacute;lico. Dise&ntilde;ado para sistemas de recolecci&oacute;n de basura.</td>
<td style="height: 13px; width: 289.531px;">Reforzado, met&aacute;lico. Utilidad campos, barrios cerrados, municipios. Alta capacidad.&nbsp;&nbsp;</td>
</tr>
</tbody>
</table>
<p align="center"><em>Tabla 1: Comparaci&oacute;n de productos de mercado</em></p>

## Capítulo 2. Introducción específica

### 2.1. Requisitos

En la tabla 2 se muestran los requisitos del sistema desarrollado.

<table>
<thead>
<tr style="height: 13px;">
<th style="height: 13px;">Grupo de requisitos</th>
<th style="height: 13px;">Id</th>
<th style="height: 13px;">Descripci&oacute;n</th>
</tr>
</thead>
<tbody>
<tr style="height: 13px;">
<td style="height: 65px;" rowspan="5">1. Control</td>
<td style="height: 13px;">1.1</td>
<td style="height: 13px;">El sistema deber&aacute; bloquear la tapa si el contenedor esta lleno.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">1.2</td>
<td style="height: 13px;">El sistema deber&aacute; abrir la tapa si detecta la presencia de un usuario.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">1.3</td>
<td style="height: 13px;">El sistema deber&aacute; cerrar la tapa si esta en ausencia de un usuario.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">1.4</td>
<td style="height: 13px;">La tapa deber&aacute; poder desbloquearse por medio de Wi-Fi o UART</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">1.5</td>
<td style="height: 13px;">El contenedor deber&aacute; encender una alarma auditiva si se detecta gas metano.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 39px;" rowspan="3">2. Interacci&oacute;n con el usuario</td>
<td style="height: 13px;">2.1</td>
<td style="height: 13px;">El usuario puede controlar el contenedor desde una pagina web.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">2.2</td>
<td style="height: 13px;">El usuario puede consultar el estado desde una pagina web, o consultar el display del contenedor.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">2.3</td>
<td style="height: 13px;">El display se enciende solo bajo la presencia de un usuario.</td>
</tr>
<tr style="height: 13.6528px;">
<td style="height: 65.6528px;" rowspan="4">3. El servicio web</td>
<td style="height: 13.6528px;">3.1</td>
<td style="height: 13.6528px;">El servicio web puede ser accedido desde un browser.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">3.2</td>
<td style="height: 13px;">El servicio web ofrece un men&uacute; de acciones para controlar el contenedor.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">3.3</td>
<td style="height: 13px;">El servicio web ofrece una api web para controlar el contenedor de forma programable.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">3.4</td>
<td style="height: 13px;">El servicio web ofrece m&eacute;tricas del estado del contenedor.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 26px;" rowspan="2">4. Alimentaci&oacute;n</td>
<td style="height: 13px;">4.1</td>
<td style="height: 13px;">La alimentaci&oacute;n es por medio de una bater&iacute;a de 5 V.</td>
</tr>
<tr style="height: 13px;">
<td style="height: 13px;">4.2</td>
<td style="height: 13px;">La carga de la bater&iacute;a es por medio de un panel solar.</td>
</tr>
</tbody>
</table>
<p align="center"><em>Tabla 2: Requisitos del proyecto</em></p>

**Tabla 2.1: Requisitos del sistema automático.**

### 2.2. Casos de uso
En las tablas 3, 4 y 5 se presentan tres casos de uso del sistema representativos de su funcionalidad.

---

| Elemento | Definición |
|----------|------------|
| Disparador | El usuario se acerca al contenedor |
| Precondición | El contenedor esta cerrado |
| Flujo básico | 1. El contenedor detecta la presencia del usuario. <br> 2. El contenedor abre la tapa. <br> 3. El usuario deposita la basura. <br> 4. El usuario se aleja del contenedor. <br> 5. El contenedor cierra la tapa. |
| Flujo alternativo | 1. El contenedor detecta la presencia del usuario. <br> 2. El display indica al usuario que esta lleno. <br> 3. El usuario lee el display. <br> 4. El usuario se aleja del contenedor. |
<p align="center"><em>Tabla 3: Caso de uso 1: Un usuario arroja un residuo.</em></p>

---

| Elemento | Definición |
|----------|------------|
| Disparador | El contenedor detecta gas metano |
| Precondición | El contenedor esta cerrado |
| Flujo básico | 1. El contenedor detecta gas metano. <br> 2. El contenedor emite una alarma auditiva. <br> 3. La página web cambia el estado del contenedor a "Alerta". |
| Flujo alternativo | 1. El contenedor detecta gas metano. <br> 2. El contenedor emite una alarma auditiva. <br> 3. La página web cambia el estado del contenedor a "Alerta". <br> 4. Un servidor llama a la api y detecta la alerta. <br> 5. El servidor envía un mail al encargado de la recolección. |

<p align="center"><em>Tabla 4: Caso de uso 2: Gas Metano.</em></p>

---

| Elemento | Definición |
|----------|------------|
| Disparador | El contenedor se llenó |
| Precondición | El contenedor esta cerrado |
| Flujo básico | 1. El contenedor detecta que esta lleno. <br> 2. El contenedor bloquea la tapa. <br> 3. La página web cambia el estado del contenedor a "Lleno". |
| Flujo alternativo | 1. El contenedor detecta que esta lleno. <br> 2. El contenedor bloquea la tapa. <br> 3. La página web cambia el estado del contenedor a "Lleno". <br> 4. Un servidor llama a la api y detecta el estado. <br> 5. El servidor envía un mail al encargado de la recolección. <br> 6. El recolector se hacerca al contenedor, desbloquea al contenedor con la página web y recolecta la basura. |
<p align="center"><em>Tabla 5: Caso de uso 3: Contenedor lleno.</em></p>


### 2.3. Descripción de módulos utilizado

En base a la arquitectura de control y los requisitos establecidos se decidió por utilizar los módulos que se
describen a continuación.

#### 2.3.1. Placa Nucleo F429ZI
Este es el microcontrolador principal utilizado en el proyecto. Se programó con el lenguaje C++ y el framework MBed-OS. Este módulo superó ampliamente los requerimientos del proyecto gracias a su gran cantidad de puertos y caracteristicas disponibles de prototipado.

<p align="center">
    <img alt="" src="Images/nucleoFZ.jpg" width="600">
</p>

<p align="center"><em>Figura 1: Placa Núcleo F429ZI</em></p>

#### 2.3.2. Módulo del display grafico
Para la presentación de información al usuario se utilizó un display 1602 azul junto con un módulo I2C para recibir los datos desde la placa. 

<p align="center">
    <img alt="" src="Images/Display.png" width="600">
</p>

<p align="center"><em>Figura 2: Modulo display 1602 con módulo I2C.</em></p>

#### 2.3.3. Módulo Wi-Fi
Para la implementación del servidor web se utilizó la ESP01. La pagina web se almacenó en la placa núcleo. La comunicación con la placa principal se dió por medio de una conexión UART y comandos AT.

<p align="center">
    <img alt="" src="Images/Esp01.jpg" width="300">
</p>


<p align="center"><em>Figura 3: Modulo Wi-Fi ESP01 - ESP8266</em></p>

#### 2.3.4.  Sensor presencia (Sensor PIR)
El modulo HC-Sr501 se utilizó para detectar la presencia de un usuario. Al acercar la mano, el sensor devulve un estado alto. 

<p align="center">
    <img alt="" src="Images/sensorPIR.png">
</p>

<p align="center"><em>Figura 4: sensor PIR.</em></p>

#### 2.3.5.  Sensor de gas MQ-2
Este sensor se utilizó para detectar la presencia de gas. 

<p align="center">
    <img alt="" src="Images/Mq2.jpg">
</p>

<p align="center"><em>Figura 5: sensor PIR.</em></p>

#### 2.3.5.  Sensor de temperatura LM-35
Este sensor se utilizó para obtener la temperatura del ambiente. Se añado un circuito para amplificar la salida del sensor y ajustarla a un valor que puediera ser mejor interpretado por la placa núcleo. 

<p align="center">
    <img alt="" src="Images/TL35.webp">
</p>

<p align="center"><em>Figura 5: sensor PIR.</em></p>

#### 2.3.6.  Motor a pasos 28BYJ-48
Motor a pasos utilizado como actuador de la tapa para abrir y cerrar. 

<p align="center">
    <img alt="" src="Images/Motorapasos.jpg">
</p>

<p align="center"><em>Figura 6: sensor PIR.</em></p>


## Capítulo 3. Diseño e implementación

### 3.1.Hardware

#### 3.1.1. Diagrama en bloques

En la Figura 7 se muestra un diagrama del hardware del sistema desarrollado.

<p align="center">
    <img alt="" src="Images/DiagramaEnBloques.svg">
</p>
<p align="center"><em>Figura 7: Diagrama en bloques del sistema</em></p>


#### 3.1.2. Lista de señales
En la tabla 6 se listan las señales del sistema, indicando la conexión de los puertos de la placa NUCLEO-
F429ZI a los módulos de hardware.

<table style="width: 415px;">
<thead>
<tr style="height: 23px;">
<th style="height: 23px; width: 201px;" colspan="2">Pin del m&oacute;dulo de hardware</th>
<th style="height: 23px; width: 208px;" colspan="2">Pin de la placa Nucleo-F429ZI</th>
<th style="height: 23px; width: 159.549px;" colspan="3">Fuente de Alimentacion</th>
</tr>
</thead>
<tbody>
<tr style="height: 23px;">
<td style="height: 92.1945px; width: 132px;" rowspan="4">Display 1602</td>
<td style="height: 23px; width: 69px;">SCL</td>
<td style="height: 23px; width: 69px;">PB_8</td>
<td style="height: 23px; width: 139px;">I2C1_SCL</td>
<td style="height: 23px; width: 139px;">&nbsp;-</td>
</tr>
<tr style="height: 23.1945px;">
<td style="height: 23.1945px; width: 69px;">SDA</td>
<td style="height: 23.1945px; width: 69px;">PB_9</td>
<td style="height: 23.1945px; width: 139px;">I2C1_SDA</td>
<td style="height: 23.1945px; width: 139px;">-&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">VCC</td>
<td style="height: 23px; width: 69px;">-</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">&nbsp;5V</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">GND</td>
<td style="height: 23px; width: 69px;">-</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">&nbsp;GND</td>
</tr>
<tr style="height: 23px;">
<td style="height: 69px; width: 132px;" rowspan="3">Sensor PIR</td>
<td style="height: 23px; width: 69px;">VCC</td>
<td style="height: 23px; width: 69px;">5V</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">&nbsp;5V</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">SIGNAL</td>
<td style="height: 23px; width: 69px;">PE_13</td>
<td style="height: 23px; width: 139px;">Digital - PullDown</td>
<td style="height: 23px; width: 139px;">&nbsp;-</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">GND</td>
<td style="height: 23px; width: 69px;">GND</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="height: 92px; width: 132px;" rowspan="3">LM-35 (Temperatura)</td>
<td style="height: 23px; width: 69px;">Out</td>
<td style="height: 23px; width: 69px;">A1</td>
<td style="height: 23px; width: 139px;">&nbsp;Analog In</td>
<td style="height: 23px; width: 139px;">&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">VIN</td>
<td style="height: 23px; width: 69px;">-</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">&nbsp;5V</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">GND</td>
<td style="height: 23px; width: 69px;">-</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">GND</td>
</tr>
<tr style="height: 23px;">
<td style="height: 92px; width: 132px;" rowspan="3">MQ-2 (Sensor de gas)</td>
<td style="height: 23px; width: 69px;">Out</td>
<td style="height: 23px; width: 69px;">A0</td>
<td style="height: 23px; width: 139px;">&nbsp;Analog In</td>
<td style="height: 23px; width: 139px;">&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">VIN</td>
<td style="height: 23px; width: 69px;">-</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">&nbsp;5V</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">GND</td>
<td style="height: 23px; width: 69px;">-</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">GND</td>
</tr>
<tr style="height: 23px;">
<td style="height: 92px; width: 132px;" rowspan="3">Motor a pasos 28BYJ-48</td>
<td style="height: 23px; width: 69px;">In1-In4</td>
<td style="height: 23px; width: 69px;">D4-D7</td>
<td style="height: 23px; width: 139px;">&nbsp;Bus Digital Out 4bits</td>
<td style="height: 23px; width: 139px;">&nbsp;-</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">VIN</td>
<td style="height: 23px; width: 69px;">-</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">&nbsp;12V</td>
</tr>
<tr style="height: 23px;">
<td style="height: 23px; width: 69px;">GND</td>
<td style="height: 23px; width: 69px;">-</td>
<td style="height: 23px; width: 139px;">-</td>
<td style="height: 23px; width: 139px;">GND</td>
</tr>
</tbody>
</table>
<p>&nbsp;</p>
<p align="center"><em>Tabla 6: Lista de se&ntilde;ales del sistema</em></p>

### 3.2. Firmware

#### 3.2.1. Repositorio
Todo el código del proyecto se encuentra en el repositorio git en [4].

#### 3.2.2. Tecnologı́a
El sistema se encuentra implementado en C++ utilizando Mbed. El firmware presenta un archivo main.cpp el cual lo único que realiza es llamar a las funciones inicio de sistema, y en el lazo principal, la funcion de actualizacion del sistema.

#### 3.2.3. Estructura del repositorio


| Directorio/Archivo        | Contenido principal                                          |
|-------------------|--------------------------------------------------------------|
| `/`            | Archivos fuente del proyecto.                                 |
| `/Images/`       | Imagenes y diagramas del proyecto.      |
| `/SiMBa/`    | Módulos principales del sistema embebido, lógica de control y organización del código fuente. |
| `/SiMBa/simba.cpp` | Lógica principal y configuración del sistema, inicialización de módulos, gestión de la máquina de estados y flujo principal del programa.     |
| `/SiMBa/Modulos`    | Módulos de actuadores y sensores, cada uno encapsula la lógica para interactuar con un componente específico (motores, sensores, display, etc.).         |
| `/SiMBa/Modulos/PCCom`    | Funciones para comunicación y escritura por puerto serie con la PC, útil para depuración y monitoreo desde el ordenador.                |
| `/SiMBa/Modulos/WebFramework`    |Definiciones de la página web embebida; en views se definen todas las vistas del proyecto y el comportamiento de la API para interacción remota. |
| `/SiMBa/Modulos/Wi-Fi`    | Módulo con la maquina de estados del modulo Wifi|
| `/main.cpp`    | Archivo principal de ejecución          |
| `/mbed_app.json`    | Archivo de configuracion para el compilador     |

<p align="center"><em>Tabla 7: Estructura de directorios y modulos</em></p>


| Nombre de elemento        | Tipo                          |      Descripción   |
|-------------------|-----------------------|---------------------------------------|
| machine       | Objeto Maquina de estados      | Contiene una instancia del objeto ´maquina de estados´ utilizado para la definicion de los estados, las transiciones e inicialización de los sensores y actuadores utilizados.      |
<p align="center"><em>Tabla 8: Objetos y Variables del modulo simba.cpp</em></p>

| Nombre de elemento        | Tipo                          |      Descripción   |
|-------------------|-----------------------|---------------------------------------|
| machine       | Objeto Maquina de estados      | Contiene una instancia del objeto ´maquina de estados´ utilizado para la definicion de los estados, las transiciones e inicialización de los sensores y actuadores utilizados.      |
<p align="center"><em>Tabla 9: Objetos y Variables del modulo wi-fi.cpp</em></p>

| Nombre de elemento        | Tipo                          |      Descripción   |
|-------------------|-----------------------|---------------------------------------|
| actuadorAlarma.cpp       | Objeto DigitalOut     | Es una salida digital que controla una alarma.|
| actuadorTapa.cpp       | Dos objetos digital In y un digital out     | Monitorea el estado de la tapa y actua sobre el motor a pasos. |
| display.cpp       | Objeto i2c     | Actua sobre el display, ya sea su iluminación y los caracteres que muestra.|
| maquinaDeEstados.cpp       | Objeto nuevo maquina de estados      | Mini framework para definir una maquina de estados. |
| motor.cpp       | Objeto Bus Digital Out      | Es una salida que actua sobre un motor a pasos. |
| sensorDeGas.cpp       | Objeto analog In      | Sirve para monitorear el estado del sensor de gas.|
| sensorDeNivel.cpp       | Objeto Digital In     | Lee el estado de un switch utilizado para detectar si el tacho está lleno o vacío.|
| sensorDePresencia.cpp       | Objeto Digital In     | Es una entrada que monitorea el estado de un sensor pir para indicar si hay un usuario conectado.|
| sensorDeTemperatura.cpp       | Objeto Analog In      | Es una entrada que monitorea la temperatura del tacho. |
| temporizador.cpp       | Objeto Ticker      | Clase con métodos para definir temporizadores. |
| uart.cpp       | Objeto Serial Bus     | Clase con métodos para transmitir con la uart. |

<p align="center"><em>Tabla 10: Carpeta módulos</em></p>


| Nombre de elemento        | Tipo                          |      Descripción   |
|-------------------|-----------------------|---------------------------------------|
| Views     | Clases varias      | Definen el esqueleto de una pagina web. |
| app.cpp       | Objeto       | Define una aplicación web.|
| routes.cpp       | Objeto      | Define el ruteo de la aplicación. |


<p align="center"><em>Tabla 11: Carpeta WebFramework</em></p>




#### 3.2.6. Arquitectura
En la figura 3.3 se muestra el diagrama de flujo del firmware.

<p align="center">
    <img alt="" src="Images/TP3_DiagramaDeEstados.png">
</p> 

<p align="center"><em>Figura 3.3: Diagrama de flujo principal del firmware</em></p>

## Capítulo 4. Ensayos y resultados

### 4.1. Pruebas funcionales del hardware
Las pruebas funcionales del hardware se realizaron por módulos.

### 4.1.1. Módulo Wi-Fi NODEMCU8266
Se cargo el firmware AT en el modulo. Luego por conexion USB y comunicacion por el puerto serie se constato el correcto envio de comandos y sus respuestas. Ademas, mediante esos comandos se estableció conexion con la red usada por defecto.

### 4.1.2. Módulo Sesor de pulso HW-827
El estudio inició con la toma de una medida de la señal analógica a través de un osciloscopio, con el propósito de analizar tanto los niveles como el comportamiento de dicha señal durante la lectura de pulsos. Para ello, se procedió a la conexión de la señal a los 3,3 V y GND de la placa núcleo, evidenciándose la presencia de picos de amplitud asociados a la detección de pulsos, junto con la observación de ruido de interferencia superpuesto en la señal. 
Posteriormente, se llevó a cabo un experimento adicional empleando la placa núcleo junto con el software SerialPlot. A través de esta herramienta, se lograron determinar la frecuencia de muestreo óptima, los umbrales de detección y los filtros necesarios para asegurar una correcta lectura de pulsos.

### 4.1.3. Modulo Display OLED SSD1306

Para este caso se evaluaron las funciones desarrolladas de escritura y borrado, visualizando la pantalla del display. Se constato que los datos en la pantalla fueran los correctos y luego el borrado de pantalla.

### 4.1.8. Pruebas de integracion
Las pruebas de integración realizadas se encuentran en formato de video en el siguiente enlace:

[![Campo Alt](https://img.youtube.com/vi/MNq_PzCt46s/0.jpg)](https://www.youtube.com/watch?v=MNq_PzCt46s)


Donde se verificó:
* Disposición del hardware.
* Lógica del funcionamiento del sistema.
* Comandos por puerto serie.
* Monitoreo mediante el servidor web.
* Alertas de umbrales.
* Registro de datos historicos.

### 4.1.9. Cumplimiento de requisitos
En la tabla 4.1 se presenta la evaluación del cumplimiento de los requisitos iniciales de la tabla 2.1. Se evaluó
a el estado actual de cada uno indicando en verde aquellos que ya fueron cumplidos y en rojo los requerimientos
no cumplidos.


<table class="tg"><thead>
  <tr>
    <th class="tg-fymr">Requisito</th>
    <th class="tg-fymr">Comentarios</th>
  </tr></thead>
<tbody>
  <tr>
    <td class="tg-x4od">1.1 :green_circle: El sistema deberá bloquear la tapa si el contenedor esta lleno.</td>
    <td class="tg-0pky">Implementado.</td>
  </tr>
  <tr>
    <td class="tg-xzpz">1.2 :green_circle: El sistema deberá abrir la tapa si detecta la presencia de un usuario.</td>
    <td class="tg-0pky">Implementado.</td>
  </tr>
  <tr>
    <td class="tg-xzpz">1.3 :green_circle: El sistema deberá cerrar la tapa si esta en ausencia de un usuario.</td>
    <td class="tg-0pky"><span style="font-weight:400;font-style:normal">Implementado.</span></td>
  </tr>
  <tr>
    <td class="tg-bw5o">1.4 ⚫ La tapa deberá poder desbloquearse por medio de Wi-Fi o UART</td>
    <td class="tg-0pky">Este punto se descartó para reducir el alcance del trabajo final.</td>
  </tr>
  <tr>
    <td class="tg-xzpz">1.5 :green_circle: El contenedor deberá encender una alarma auditiva si se detecta gas metano.</td>
    <td class="tg-0pky"><span style="font-weight:400;font-style:normal">Implementado.</span></td>
  </tr>
  <tr>
    <td class="tg-xzpz">2.1 :green_circle: El usuario puede controlar el contenedor desde una pagina web.</td>
    <td class="tg-0pky">Implementado.</td>
  </tr>
  <tr>
    <td class="tg-xzpz">2.2 :green_circle: El usuario puede consultar el estado desde una pagina web, o consultar el display del contenedor.</td>
    <td class="tg-0pky">Implementado.</td>
  </tr>
  <tr>
    <td class="tg-8eqh">2.3 :green_circle: El display se enciende solo bajo la presencia de un usuario.</td>
    <td class="tg-0pky">El display se enciende bajo la presencia de un usuario.</td>
  </tr>
  <tr>
    <td class="tg-bw5o">3.1 ⚫ El servicio web tiene usuario y contraseña.</td>
    <td class="tg-0pky">Este punto se descartó para reducir el alcance del trabajo final.</td>
  </tr>
  <tr>
    <td class="tg-bw5o">3.2 :green_circle: El servicio web puede ser accedido desde un browser.</td>
    <td class="tg-0pky">Se implementó un servicio web con un home desde donde se pueden acceder a algunas acciones del tacho de basura.</td>
  </tr>
  <tr>
    <td class="tg-hrbo">3.3 :green_circle: El servicio web ofrece un menú de acciones para controlar el contenedor.</td>
    <td class="tg-0pky">Con el servicio web se puede controlar la tapa.</td>
  </tr>
  <tr>
    <td class="tg-hrbo">3.4 :green_circle: El servicio web ofrece una api web para controlar el contenedor de forma programable.</td>
    <td class="tg-0pky">Se agregó la posibilidad de realizar acciones sobre el contenedor mediante una api web que actualmente incluye control sobre la tapa.</td>
  </tr>
  <tr>
    <td class="tg-hrbo">3.5 :green_circle: El servicio web ofrece métricas del estado del contenedor.</td>
    <td class="tg-0pky">El servicio web ofrece la temperatura.</td>
  </tr>
  <tr>
    <td class="tg-hrbo">4.1 ⚫ La alimentación es por medio de una batería de 5 V.</td>
    <td class="tg-0pky">Este punto se descartó para reducir el alcance del trabajo final.</td>
  </tr>
  <tr>
    <td class="tg-hrbo">4.2 ⚫ La carga de la batería es por medio de un panel solar.</td>
    <td class="tg-0pky">Este punto se descartó para reducir el alcance del trabajo final.</td>
  </tr>
</tbody></table>

<p align="center"><em>Tabla 4.1: Estado de requisitos.</em></p>



### 4.1.10. Comparación con otros sistemas similares

En la Tabla 4.2 se puede observar la continuación del análisis de la Sección 1.2, donde se puede ver ahora sumado 
a la comparación al sistema de monitoreo realizado.

<table border="1" cellspacing="0" cellpadding="5">
<thead>
<tr>
<th>Caracter&iacute;stica</th>
<th>[MAGENE H303](https://www.magene.com/en/sensors/52-h303-heart-rate-monitor.html)</th>
<th>[POLAR Verity Sense](https://www.polar.com/us-en/products/accessories/polar-verity-sense)</th>
<th>[WELLUE O2Ring](https://getwellue.com/pages/o2ring-oxygen-monitor)</th>
<th>&nbsp;</th>
<th>[Wellue Oxiband](https://www.mercadolibre.com.ar/oximetro-de-pulso-wellue-oxiband-con-app-y-recordatorio/p/MLA50740493)</th>
<th>Sistema de monitoreo de frecuencia cardiaca (Este proyecto)</th>
</tr>
</thead>
<tbody>
<tr>
<td>Tipo de sensor</td>
<td>Banda tor&aacute;cica con sensor ECG</td>
<td>Banda &oacute;ptica para brazo (PPG)</td>
<td>Anillo con sensor &oacute;ptico (PPG)</td>
<td>&nbsp;</td>
<td>Sensor &oacute;ptico PPG para SpO2 y pulso</td>
<td>&Oacute;ptico</td>
</tr>
<tr>
<td>Rango frecuencia card&iacute;aca</td>
<td>30 - 240 bpm</td>
<td>30 - 220 bpm</td>
<td>No especificado (pulso y SpO2)</td>
<td>&nbsp;</td>
<td>30 - 250 bpm</td>
<td>30 - 220 bpm</td>
</tr>
<tr>
<td>Duraci&oacute;n bater&iacute;a</td>
<td>Hasta 1000 horas</td>
<td>Hasta 20 horas</td>
<td>Hasta 14 horas</td>
<td>&nbsp;</td>
<td>Aproximadamente 8 horas</td>
<td>&nbsp;-</td>
</tr>
<tr>
<td>Tipo de bater&iacute;a</td>
<td>Pila bot&oacute;n CR2032</td>
<td>Bater&iacute;a recargable integrada</td>
<td>Bater&iacute;a recargable integrada</td>
<td>&nbsp;</td>
<td>Bater&iacute;a recargable integrada</td>
<td>&nbsp;-</td>
</tr>
<tr>
<td>Conectividad inal&aacute;mbrica</td>
<td>Bluetooth 4.2 y ANT+</td>
<td>Bluetooth 5.0</td>
<td>Bluetooth 4.0</td>
<td>&nbsp;</td>
<td>Bluetooth 4.0</td>
<td>Wi-Fi</td>
</tr>
<tr>
<td>Impermeabilidad</td>
<td>IP67 (resistente al agua y polvo)</td>
<td>Resistente al agua (IPX7)</td>
<td>IP24 (resistente a salpicaduras)</td>
<td>&nbsp;</td>
<td>No especificado</td>
<td>&nbsp;-</td>
</tr>
<tr>
<td>Display</td>
<td>No incluye display (se conecta a apps o dispositivos externos)</td>
<td>No incluye display (se conecta a apps)</td>
<td>No incluye display (datos en app)</td>
<td>&nbsp;</td>
<td>S&iacute;, display OLED integrado</td>
<td>&nbsp;S&iacute;, display OLED integrado</td>
</tr>
<tr>
<td>Audio / alertas</td>
<td>No incluye</td>
<td>No incluye</td>
<td>Vibraci&oacute;n para alertas</td>
<td>&nbsp;</td>
<td>Alarmas sonoras y visuales</td>
<td>Alertas visuales</td>
</tr>
<tr>
<td>Precio aproximado</td>
<td>$30 - $40 USD</td>
<td>$90 - $120 USD</td>
<td>$150 - $200 USD</td>
<td>&nbsp;</td>
<td>$600 - $800 USD</td>
<td>$30 - $52 USD</td>
</tr>
<tr>
<td>Uso principal</td>
<td>Monitoreo deportivo y fitness</td>
<td>Monitoreo deportivo y salud continua</td>
<td>Monitoreo m&eacute;dico de SpO2 y pulso</td>
<td>&nbsp;</td>
<td>Monitoreo m&eacute;dico de SpO2 y frecuencia card&iacute;aca</td>
<td>Monitoreo de salud hogareño.</td>
</tr>
</tbody>
</table>
<p align="center"><em>Tabla 4.2: Comparación de características de productos analizados previamente y este proyecto</em></p>

### 4.2. Documentación del desarrollo realizado

<p align="center">
<table><thead>
  <tr>
    <th>Elemento</th>
    <th>Referencia</th>
  </tr></thead>
<tbody>
  <tr>
    <td>Presentación del proyecto</td>
    <td>Capı́tulo 1</td>
  </tr>
  <tr>
    <td>Listado de requisitos</td>
    <td>Tabla 2.1</td>
  </tr>
  <tr>
    <td>Casos de uso del proyecto</td>
    <td>Tablas 2.2 a 2.4</td>
  </tr>
  <tr>
    <td>Diagrama en bloques del sistema</td>
    <td>Figura 3.1</td>
  </tr>
  <tr>
    <td>Lista de señales</td>
    <td>Tabla 3.1</td>
  </tr>
  <tr>
    <td>Implementación del hardware</td>
    <td>Sección 3.1</td>
  </tr>
  <tr>
    <td>Módulos de software</td>
    <td>Sección 3.2</td>
  </tr>
  <tr>
    <td>Repositorio</td>
    <td>[5]</td>
  </tr>
  <tr>
    <td>Cumplimiento de requisitos</td>
    <td>Tabla 4.1</td>
  </tr>
  <tr>
    <td>Conclusiones finales</td>
    <td>Capı́tulo 5</td>
  </tr>
</tbody>
</table>
</p>
<p align="center"><em>Tabla 4.3: Elementos del sumario del sistema automático para el sistema de monitoreo de frecuencia cardiaca</em></p>

## Capı́tulo 5 Conclusiones

### 5.1. Resultados obtenidos
El desarrollo del sistema de monitoreo de frecuencia cardíaca permitió cumplir con los objetivos planteados inicialmente. Se logró la integración exitosa de un sensor óptico de pulso, un display OLED para la visualización en tiempo real de los latidos por minuto (BPM), y un módulo de conectividad Wi-Fi para la transmisión, tanto de lecturas en tiempo real, alertas y datos históricos, en una plataforma remota.

El sistema mostró una lectura estable del pulso en tiempo real durante las pruebas. La implementación de umbrales configurables permitió activar alertas cuando la frecuencia cardíaca superó o descendió de ciertos valores establecidos, lo que demuestra su potencial como herramienta preventiva o de monitoreo continuo en contextos personales o deportivos.

Además, se logró un registro automático de los datos, lo cual facilita su análisis posterior y el seguimiento de patrones a lo largo del tiempo. La interfaz en el display OLED resultó clara y funcional para la visualización inmediata del estado del usuario.

### 5.1. Proximos pasos

Si bien el sistema ha demostrado un buen funcionamiento, se identificaron oportunidades de mejora y expansión que podrían implementarse en futuras iteraciones del proyecto:

1. Agregar una conectividad Bluetooth para tener mayor control y configuracion de redes Wi-Fi.
2. Incorporar el uso de baterias para tener un sistema portable.
1. Mejora de la precisión del sensor: Evaluar la integración de sensores ópticos más avanzados o de múltiples canales para reducir interferencias y mejorar la fiabilidad de las mediciones en distintos tipos de piel y condiciones de movimiento.
2. Almacenamiento en la nube y análisis inteligente: Incorporar servicios en la nube para almacenamiento seguro, y aplicar algoritmos de análisis de datos para detectar anomalías o tendencias relevantes en la frecuencia cardíaca del usuario.

    
## Bibliografı́a


[1] MBED OS API reference. [Mbed OS Api reference](https://os.mbed.com/docs/mbed-os/v6.16/apis/index.html)

[2] At command set. [ESP8266 At Commands.](https://docs.espressif.com/projects/esp-at/en/release-v2.2.0.0_esp8266/AT_Command_Set/TCP-IP_AT_Commands.html) 

[3] Espressif Systems. [ESP8266 Datasheet.](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf) 

[4] Agustín Ruiz. Repositorio de proyecto. [Sistema de monitoreo de basura.](https://github.com/agustin58ruiz/SE_TP1_Sistema-de-monitoreo-de-contenedores-de-basura)

[5] A. Lutenberg, P. Gómez, and E. Pernia, A Beginner’s Guide to Designing Embedded System Applications – M Cortex® Microcontrollers, ARM Education Media, 2002, ISBN: 978-1-911531-41-8 (print), 978-1-911531-42-5 (ePDF).

[6] P. Marwedel, Embedded System Design: Embedded Systems Foundations of Cyber-Physical Systems, and the Internet of Things, 4th ed. Cham, Switzerland: Springer, 2021. [Online]. Available: https://doi.org/10.1007/978-3-030-60910-8
