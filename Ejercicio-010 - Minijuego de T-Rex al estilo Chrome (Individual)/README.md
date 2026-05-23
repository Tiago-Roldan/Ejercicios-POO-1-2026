# Ejercicio 10 - T-Rex Extremo

## Descripción del Proyecto

Proyecto de minijuego inspirado en el clásico juego del T-Rex de Google Chrome. El jugador controla un dinosaurio que debe esquivar cactus y pájaros mientras intenta sobrevivir el mayor tiempo posible. El juego aumenta su dificultad progresivamente con el paso del tiempo, incrementando la velocidad de los obstáculos.

## Requisitos del Enunciado

### Estructura de Clases

- **Clase TRex**: Hereda de QWidget o QImage. Representa el dinosaurio controlable por el jugador.
- **Clase Pajaro**: Hereda de QWidget o QImage. Cada instancia de Pajaro posee su propio QTimer independiente para controlar su movimiento horizontal (hacia la izquierda).
- **Clase Cactus**: Representa los obstáculos que se desplazan horizontalmente.
- **Clase GameWidget**: Contenedor principal del juego que gestiona la lógica general.

### Sistema de Timers

- **QTimer Principal**: Controla el movimiento de los cactus y la detección de colisiones.
- **QTimer Individual por Pajaro**: Cada pájaro tiene su propio QTimer que gestiona su movimiento independiente.
- **Aparición de Pájaros**: Cada 5 segundos aparece un nuevo pájaro con su propio QTimer.

Conexión de QTimer:
```cpp
connect(timer, SIGNAL(timeout()), this, SLOT(moverPajaro()));
```

Para eventos únicos, utilizar QTimer::singleShot():
```cpp
QTimer::singleShot(5000, this, SLOT(crearNuevoPajaro()));
```

### Controles del Juego

El jugador controla al dinosaurio mediante las siguientes teclas:

- **Espacio**: Saltar (permite esquivar cactus bajos).
- **Flecha Abajo**: Agacharse (permite esquivar pájaros).
- **Flecha Derecha**: Adelantarse (aumenta velocidad de movimiento).
- **Flecha Izquierda**: Frenarse (disminuye velocidad de movimiento).

### Mecánica de Juego

- **Detección de Colisiones**: Se implementa detección de colisiones entre el dinosaurio y los cactus, así como entre el dinosaurio y los pájaros.
- **Game Over**: Cuando el dinosaurio colisiona con cualquier obstáculo, el juego termina. Se muestra una pantalla de "Game Over" y se permite reiniciar el juego.
- **Aumento de Dificultad**: La velocidad de los cactus y pájaros aumenta progresivamente conforme avanza el juego.
- **Variedad de Pájaros**: Los pájaros aparecen con alturas aleatorias para aumentar la complejidad.

## Instrucciones de Compilación y Ejecución

### Requisitos Previos

- Qt Framework 6.x o superior
- Qt Creator
- Compilador MinGW 64-bit (recomendado)

### Abrir el Proyecto en Qt Creator

1. Abre Qt Creator.
2. Selecciona Archivo > Abrir archivo o proyecto.
3. Navega a la carpeta del proyecto y selecciona el archivo `Proyecto_10.pro`.
4. Qt Creator cargará automáticamente la configuración del proyecto.
5. Selecciona el kit de compilación (Desktop Qt 6.11.0 MinGW 64-bit - Debug).

### Compilar el Proyecto

Una vez cargado el proyecto en Qt Creator:

1. Selecciona Compilación > Compilar todo (o presiona Ctrl+Shift+B).
2. Espera a que la compilación finalice sin errores.
3. Los archivos compilados se generarán en la carpeta `build/`.

### Ejecutar el Juego

1. Presiona Ctrl+R o selecciona Compilación > Ejecutar.
2. La ventana del juego se abrirá y el juego estará listo para comenzar.

## Detalles de Implementación

### Gestión de Timers

La gestión correcta de QTimer es fundamental para el funcionamiento del juego:

- **QTimer Principal**: Gestiona el movimiento de los cactus y verifica colisiones a intervalos regulares.
- **QTimer de Pájaro**: Cada instancia de Pájaro contiene su propio QTimer que mueve la posición horizontal del pájaro hacia la izquierda.
- **Creación Periódica de Pájaros**: Se utiliza QTimer::singleShot() o un temporizador auxiliar para crear nuevos pájaros cada 5 segundos.

### Detección de Colisiones

La detección de colisiones se realiza comparando las áreas de los objetos en pantalla. El QTimer principal invoca un slot de verificación de colisiones en intervalos regulares.

### Aumento de Velocidad

A medida que el juego progresa, se incrementa la velocidad de movimiento de cactus y pájaros, aumentando la dificultad dinámicamente.

### Variedad de Pájaros

Los pájaros aparecen a diferentes alturas aleatorias para proporcionar una mayor variedad de desafíos al jugador.

## Consideraciones de Diseño

- **Modularidad**: Las clases están organizadas de forma modular para facilitar el crecimiento y mantenimiento del proyecto.
- **Independencia de Timers**: Cada pájaro opera de forma independiente gracias a su propio QTimer, permitiendo comportamientos asíncronos.
- **Signals y Slots**: Se utilizan los mecanismos de signals y slots de Qt para la comunicación entre componentes.

## Notas de Desarrollo

- Asegurar que todos los QTimer se conecten correctamente utilizando la sintaxis de connect() con SIGNAL y SLOT.
- Validar que la detección de colisiones funciona correctamente con todos los tipos de obstáculos.
- Probar el aumento de velocidad para garantizar que el juego no se vuelve injugable en etapas avanzadas.
- Verificar que el reinicio del juego limpia correctamente todos los timers y objetos antes de iniciar una nueva partida.
