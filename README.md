Estacion Meteorologica conectada a la nube con ESP32
============= 
Lectura de anemometro, pluviometro y veleta para estacion meteorologica con conexion a nube ThingSpeak

Este proyecto está realizado para poder leer, interpretar los datos, y subirlos a la nube con una placa de desarrollo ESP32. Por ahora la placa fue realizado en un PCB de prototipado, el siguiente paso sería realizar una pcb con la alimentación por baterías recargables o 5V, y otro paso sería agregar más sensores Como por ejemplo de temperatura y humedad, presión barométrica, etc. Cómo adquirimos solo los sensores de la estación meteorológica y no había librerías disponibles tuvimos que realizar todo el código en el Arduino IDE utilizando las interrupciones externas y conversores analógico digital para el caso de la veleta. El producto que adquirimos se encuentra en el siguiente link:

[Sensores Estacion Meteorologica](https://dynamoelectronics.com/tienda/kit-medidor-meteorologico/)

Como estos sensores tienen conexión a ficha RJ11 se decidió conseguir la respectiva ficha hembra para poderlos conectar al microcontrolador. Si no tienen fichas hembra de rj 11 podrían cortar el cable y soldarlo directamente a la placa.
Para poder usar la conexión a ThingSpeak se deberá descargar la librería correspondiente en el Arduino IDE. Una recomendación a la hora de elegir los GPIO del ESP32 es verificar en el modelo que se tiene cuáles fines están afectados durante el reinicio. Algunos GPIO cambien de estado HIGH o salida PPWM durante el reinicio de la placa. Esto puede generar problemas y resultados inesperados durante el arranque o incluso impedir el reinicio de la placa. Los pines que modifican así su estado son los siguientes:
- GPIO 1
- GPIO 3
- GPIO 5
- GPIO 6 to GPIO 11
- GPIO 14
- GPIO 15
