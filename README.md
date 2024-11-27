# SiMBa (Sistama de Monitoreo de Basura)

## Alumno: Agustín Ruiz
## Fecha: 2. º cuatrimestre 2024

## 1. Selección del proyecto a implementar
Se proponen dos proyectos para la implementación de un sistema embebido. En primer lugar se propone un sistema de monitoreo de basura que permita llevar un registro en tiempo real del estado de los contenedores de basura. Por otro lado, se propone un sistema de monitoreo de la calidad del aire en una habitación de hopital para detectar la presencia de gases tóxicos y asegurar la asepsia del ambiente.

### 1.1 Sitema de monitoreo de basura
La recolección de basura en la Ciudad de Buenos Aires es compleja y requiere de una inversión en recursos elevada. La recolección de residuos se realiza de forma agendada en donde los recolectores realizan una ruta definida. Debido a que los containedores no siempre están cargados, o están cargados de más, se propone un sistema de monitoreo para llevar un registro en tiempo real del estado de los contenedores. El sistema se compone de dos partes. Por un lado, un sistema embebido capaz de realizar un sondeo de los parámetros del contenedor tales como: capacidad, humedad, detección de gas, temperatura, presión, y también de accionar algunos mecanismos a implementar en el contendor como ventilación, bloqueo de la tapa, indicador de bateria. Por otro lado, se dispondrá de una central de datos que recepcionará todas las variables de los dispositivos que permitirá la implementación de rutas de recolección dinámicas, analisis de datos y utilización eficiente de los recursos. De esta forma se puede dar un mejor seguimiento al problema ambiental de la recolección de basura y una mejora en la eficacia de las políticas ambientales de la ciudad.

Con este proyecto se busca mejorar la eficiencia de la recolección de basura y reducir los costos asociados a la misma. También se busca que se lleve un registro del estado los contenedores y se realice un seguimiento de las rutas de recolección que permitan una mejor utilización de los recursos. 

### 1.2 Sistema de monitoreo de calidad del aire en una habitación de hospital

La calidad del aire en una habitación de hospital es un factor crítico para la recuperación de los pacientes. La presencia de gases tóxicos puede ser un factor de riesgo para la salud de los pacientes. Se propone un sistema de monitoreo de la calidad del aire en una habitación de hospital que permita detectar la presencia de gases tóxicos y asegurar la asepsia del ambiente. El sistema se compone de un sistema embebido capaz de realizar un sondeo de los parámetros del aire tales como: concentración de oxígeno, concentración de dióxido de carbono, concentración de monóxido de carbono, concentración de ozono, concentración de dióxido de nitrógeno, concentración de dióxido de azufre, concentración de partículas en suspensión, temperatura y humedad. Por otro lado, se dispondrá de una central de datos que recepcionará todas las variables de los dispositivos que permitirá la implementación de políticas de control de calidad del aire en la habitación de hospital. De esta forma se puede dar un mejor seguimiento al problema de la calidad del aire en las habitaciones de hospital y una mejora en la eficacia de las políticas de asepsia en los hospitales. 

Con este proyecto se busca mejorar la calidad de vida de los pacientes y reducir los riesgos de infección en los hospitales. 

<table border="0" cellpadding="0" cellspacing="0" width="526" style="border-collapse:
 collapse;table-layout:fixed;width:396pt">
 <colgroup><col width="209" style="mso-width-source:userset;mso-width-alt:7643;width:157pt">
 <col width="82" style="mso-width-source:userset;mso-width-alt:2998;width:62pt">
 <col width="77" style="mso-width-source:userset;mso-width-alt:2816;width:58pt">
 <col width="74" style="mso-width-source:userset;mso-width-alt:2706;width:56pt">
 <col width="84" style="mso-width-source:userset;mso-width-alt:3072;width:63pt">
 </colgroup><tbody><tr height="39" style="mso-height-source:userset;height:29.25pt">
  <td rowspan="2" height="77" class="xl69" width="209" style="height:57.75pt;
  width:157pt">Proyectos<span style="mso-spacerun:yes">&nbsp;</span></td>
  <td colspan="2" class="xl74" width="159" style="border-left:none;width:120pt">Sistema
  de monitoreo de basura</td>
  <td colspan="2" class="xl74" width="158" style="border-right:1.0pt solid black;
  border-left:none;width:119pt">Sistema de monitoreo de calidad de aire</td>
 </tr>
 <tr height="38" style="mso-height-source:userset;height:28.5pt">
  <td height="38" class="xl71" style="height:28.5pt;border-top:none;border-left:
  none">Puntuación</td>
  <td class="xl72" width="77" style="border-top:none;border-left:none;width:58pt">Puntuación
  ponderada</td>
  <td class="xl71" style="border-top:none;border-left:none">Puntuación</td>
  <td class="xl73" width="84" style="border-top:none;border-left:none;width:63pt">Puntuación
  ponderada</td>
 </tr>
 <tr height="20" style="mso-height-source:userset;height:15.0pt">
  <td height="20" class="xl66" width="209" style="height:15.0pt;border-top:none;
  width:157pt">Disponibilidad del hardware (10)</td>
  <td class="xl65" style="border-top:none;border-left:none">10</td>
  <td class="xl65" style="border-top:none;border-left:none">100</td>
  <td class="xl65" style="border-top:none;border-left:none">6</td>
  <td class="xl67" style="border-top:none;border-left:none">60</td>
 </tr>
 <tr height="20" style="height:15.0pt">
  <td height="20" class="xl68" style="height:15.0pt;border-top:none">Utilidad del
  proyecto (8)</td>
  <td class="xl65" style="border-top:none;border-left:none">7</td>
  <td class="xl65" style="border-top:none;border-left:none">70</td>
  <td class="xl65" style="border-top:none;border-left:none">9</td>
  <td class="xl67" style="border-top:none;border-left:none">90</td>
 </tr>
 <tr height="20" style="height:15.0pt">
  <td height="20" class="xl68" style="height:15.0pt;border-top:none">Disponibilidad
  del tiempo (7)</td>
  <td class="xl65" style="border-top:none;border-left:none">9</td>
  <td class="xl65" style="border-top:none;border-left:none">90</td>
  <td class="xl65" style="border-top:none;border-left:none">5</td>
  <td class="xl67" style="border-top:none;border-left:none">50</td>
 </tr>
 <tr height="21" style="height:15.75pt">
  <td height="21" class="xl76" style="height:15.75pt;border-top:none">Total</td>
  <td class="xl77" style="border-top:none;border-left:none">-</td>
  <td class="xl77" style="border-top:none;border-left:none">260</td>
  <td class="xl77" style="border-top:none;border-left:none">-</td>
  <td class="xl78" style="border-top:none;border-left:none">200</td>
 </tr>
 <!--[if supportMisalignedColumns]-->
 <tr height="0" style="display:none">
  <td width="209" style="width:157pt"></td>
  <td width="82" style="width:62pt"></td>
  <td width="77" style="width:58pt"></td>
  <td width="74" style="width:56pt"></td>
  <td width="84" style="width:63pt"></td>
 </tr>
 <!--[endif]-->
</tbody></table>


## 2. Elicitación de requisitor y casos de uso



