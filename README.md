# Monitor Aire – Monitor de calidad del aire

Monitor Aire es un monitor de calidad del aire basado en Arduino Uno. Lee un sensor de gases **MQ135**, muestra la lectura y el estado del aire en una **pantalla OLED de 128×64**, indica el estado con tres LEDs (verde / amarillo / rojo) y activa un **relé** para encender la ventilación cuando la calidad del aire es crítica.

![Monitor Aire wiring](docs/Diagrama%20Monitor.png)

## Características

- Período de calentamiento del sensor al iniciar, con mensaje en pantalla

- Lectura del sensor cada segundo, promediando 10 muestras para reducir el ruido

- Tres estados de calidad del aire:

  | Estado  | Lectura ADC | LED      | Relé (ventilación) |
  | ------- | ----------- | -------- | ------------------ |
  | NORMAL  | < 300       | Verde    | Apagado            |
  | ALERTA  | 300 – 500   | Amarillo | Apagado            |
  | CRÍTICO | > 500       | Rojo     | **Encendido**      |

- Visualización en tiempo real en la OLED: valor ADC y estado actual

- Registro por puerto serie (9600 baudios)

## Hardware

- Arduino Uno (ATmega328P)
- Módulo sensor de gases MQ135
- Pantalla OLED SSD1306 de 128×64, I2C (dirección `0x3C`)
- 3 LEDs (verde, amarillo, rojo) con resistencias de 220 Ω
- Módulo relé de 1 canal
- Ventilador o sistema de ventilación conectado a través del relé

## Conexiones

| Componente   | Pin del componente | Arduino Uno |
| ------------ | ------------------ | ----------- |
| MQ135        | AO                 | A0          |
| MQ135        | VCC / GND          | 5V / GND    |
| OLED         | SDA                | A4          |
| OLED         | SCL                | A5          |
| OLED         | VCC / GND          | 5V / GND    |
| LED verde    | Ánodo (con 220 Ω)  | D5          |
| LED amarillo | Ánodo (con 220 Ω)  | D6          |
| LED rojo     | Ánodo (con 220 Ω)  | D7          |
| Relé         | IN                 | D8          |
| Relé         | VCC / GND          | 5V / GND    |

Los cátodos de los LEDs van a GND.

## Estructura del proyecto

```
MonitorAire/
├── platformio.ini     # Configuración de PlatformIO (placa, librerías)
├── src/
│   └── main.cpp       # Código del programa
├── diagram.json       # Circuito de Wokwi para la simulación
└── wokwi.toml         # Configuración de Wokwi (rutas del firmware)
```

## Requisitos

El código es Arduino estándar, así que no depende de un IDE específico. Se puede usar cualquiera de estas opciones:

| Opción                       | Ventajas                                                                                                     | Simulación                                         |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------ | -------------------------------------------------- |
| **Arduino IDE**              | La más simple. Gratis, sin PlatformIO ni Python                                                              | No integrada (usar [wokwi.com](https://wokwi.com)) |
| **VS Code + PlatformIO IDE** | Gratis, usa el mismo `platformio.ini` y la misma estructura del proyecto. Instala PlatformIO automáticamente | Extensión **Wokwi for VS Code**                    |
| **wokwi.com** (navegador)    | Sin instalar nada, ideal para probar o hacer una demostración                                                | Integrada                                          |
| **CLion + PlatformIO**       | Buen autocompletado y depuración                                                                             | Plugin **Wokwi Simulator**                         |

Librerías necesarias:

- Adafruit SSD1306
- Adafruit GFX Library

Con PlatformIO (VS Code o CLion) se instalan automáticamente. En Arduino IDE hay que instalarlas a mano (ver más abajo).

### `platformio.ini`

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 9600
lib_deps =
    adafruit/Adafruit SSD1306
    adafruit/Adafruit GFX Library
```

## Compilar y cargar

### Arduino IDE

1. Crear un sketch nuevo y pegar el contenido de `src/main.cpp`. En Arduino IDE la línea `#include <Arduino.h>` no es necesaria, pero tampoco causa problemas.
2. En **Herramientas → Administrar bibliotecas**, instalar **Adafruit SSD1306** y **Adafruit GFX Library**.
3. Seleccionar **Herramientas → Placa → Arduino Uno** y el puerto correspondiente.
4. Presionar **Subir** y abrir el **Monitor Serie** a 9600 baudios.

### VS Code + PlatformIO

1. Instalar la extensión **PlatformIO IDE**. Esta instala PlatformIO Core automáticamente.
2. Abrir la carpeta del proyecto (**File → Open Folder**).
3. Usar los botones de la barra inferior: ✓ **Build**, → **Upload** y 🔌 **Serial Monitor**.

### CLion + PlatformIO

Requiere [PlatformIO Core](https://platformio.org/install) instalado, con el comando `pio` disponible en el PATH.

1. Abrir la carpeta del proyecto en CLion.
2. Clic derecho en `platformio.ini` → **PlatformIO → Reload PlatformIO Project**.
3. Compilar el proyecto con **Build**.
4. Para usar una placa real: conectar el Uno por USB, ejecutar **Upload** y abrir el **PlatformIO Serial Monitor** (9600 baudios).

### Desde la terminal (PlatformIO)

```
pio run                  # compilar
pio run -t upload        # cargar en la placa
pio device monitor       # monitor serie
```

## Simulación con Wokwi

El proyecto se puede probar sin hardware. En `diagram.json`, un **potenciómetro en A0** reemplaza al MQ135, lo que permite mover la lectura entre 0 y 1023.

`wokwi.toml`:

```toml
[wokwi]
version = 1
firmware = '.pio/build/uno/firmware.hex'
elf = '.pio/build/uno/firmware.elf'
```

1. Compilar el proyecto (Wokwi ejecuta el firmware compilado, así que hay que recompilar después de cada cambio).
2. Iniciar el simulador: en VS Code con **F1 → Wokwi: Start Simulator**, o en CLion con el plugin Wokwi Simulator.
3. Girar el potenciómetro y verificar los resultados de la tabla de pruebas.

También se puede simular en el navegador en [wokwi.com](https://wokwi.com): crear un proyecto de Arduino Uno, pegar el código y `diagram.json`, y agregar las dos librerías de Adafruit.

## Pruebas

| Prueba                           | Resultado esperado                                                             |
| -------------------------------- | ------------------------------------------------------------------------------ |
| Inicio                           | La OLED muestra "Calentando sensor..." y ningún LED está encendido             |
| Lectura < 300                    | LED verde, "ESTADO: NORMAL", relé apagado                                      |
| Lectura 300 – 500                | LED amarillo, "ESTADO: ALERTA", relé apagado                                   |
| Lectura > 500                    | LED rojo, "ESTADO: CRITICO", relé encendido                                    |
| La lectura vuelve a bajar de 300 | Se apagan el LED rojo y el relé                                                |
| Monitor serie                    | Una línea por segundo, por ejemplo `Lectura MQ135: 412 \| Estado: ADVERTENCIA` |

## Configuración

Estas constantes, al inicio de `main.cpp`, controlan el funcionamiento del sistema:

| Constante       | Valor por defecto | Descripción                        |
| --------------- | ----------------- | ---------------------------------- |
| `LIMITE_VERDE`  | `300`             | Límite superior del estado NORMAL  |
| `LIMITE_ROJO`   | `500`             | Límite superior del estado ALERTA  |
| `CALENTAMIENTO` | `60000` ms        | Tiempo de calentamiento del sensor |
| `INTERVALO`     | `1000` ms         | Tiempo entre lecturas              |

Los umbrales son **valores ADC sin procesar (0–1023), no ppm**. Son valores de prueba y deben calibrarse: dejar que el sensor se estabilice en aire limpio, anotar su lectura base y ajustar los límites en función de ese valor.

**Consejo:** durante las pruebas, bajar `CALENTAMIENTO` a `5000` para no esperar un minuto en cada reinicio.

## Notas

- **Lógica del relé:** muchos módulos relé se activan con señal **LOW**. Si la ventilación funciona en estado NORMAL y se detiene en CRÍTICO, hay que invertir los `HIGH`/`LOW` que se escriben en `RELE`.
- **Dirección de la OLED:** si el monitor serie muestra "Error al iniciar OLED", revisar la dirección I2C del módulo (`0x3C` o `0x3D`) con un escáner I2C.
- **MQ135:** un sensor nuevo necesita un período de quemado, idealmente 24 horas, antes de que sus lecturas sean estables. La lectura es un indicador de calidad del aire, no una medición calibrada de gases.
