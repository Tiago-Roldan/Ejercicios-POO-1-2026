# Ejercicio 01 - Planificador de trabajos prácticos (Individual)
Construir una app Qt para planificar trabajos prácticos, con login, seguimiento de entregas y persistencia local.

Alcance mínimo:

- Login con validación y usuarios locales en archivo (CSV o JSON).
- Recordar sesión de forma local (archivo simple) para no pedir login en el mismo equipo. Persistencia de 5 minutos (simulación de sesión).
- Ventana principal con tablero de trabajos prácticos en grilla (QGridLayout), con cada fila armada con QLabel y botones de acciones, y filtro por estado/prioridad.
- Alta/edición/eliminación de trabajos prácticos.
- Editor de notas asociado al trabajo práctico con guardado manual.
- Historial de acciones visible en la UI y guardado en archivo.

Condiciones:

- No usar QML.
- Usar QWidget (no QMainWindow).
- Organizar el código en clases (no todo en main.cpp).

Criterios de evaluación:

- Funciona ✔
- Código organizado ✔
- Persistencia correcta ✔
- UI usable ✔
- Explicación oral ✔

Entregables (todo en GitHub):

- Código fuente completo en el repo de estudiante.
- Capturas de: login, tablero con filtros, editor con estado guardado, historial.

___

# Funcionalidades implementadas:
- Login de usuarios mediante validacion por archivo local
- Persistencia de sesión: Se guarda la ultima sesion por un corto periodo de tiempo
- Tablero de trabajos prácticos: Utilizando QHBoxLayout y QVBoxLayout, se permite visualizar las tareas, con informacion sobre los trabajos y botones de accion en los espacios correspondientes de la grilla. Asimismo, se permite filtrar los trabajos mediante estado y prioridad
- Gestion de trabajos y notas: Se permite dar de alta un trabajo, modificarlo, asignarle y cambiarle las notas, y eliminarlo una vez se ha completado
- Historial de acciones: Visible en la pantalla principal, persiste en un archivo de guardado, y es visible en todo momento por todos los usuarios

___

# Condiciones técnicas:
- No se ha utilizado QML ni QMainWindow, se ha trabajado todo mediante QWidget, QObject, y clases con herencias de ambos
- Se ha organizado el código en diferentes archivos según la clase que usan y la funcionalidad que cumplen en el programa

___

# Capturas de pantalla requeridas:
- Login de usuarios:
  
![]()<img width="413" height="288" alt="1" src="https://github.com/user-attachments/assets/02afdc84-f5a6-432f-96ee-09a9f812e00a" />

- Tablero con filtros aplicados, historial a la derecha del mismo, y editor de notas debajo:

<img width="1273" height="793" alt="image" src="https://github.com/user-attachments/assets/747bc4d0-b414-4553-a2d0-49637025cd93" />

- Creacion de un trabajo practico nuevo:

<img width="411" height="237" alt="image" src="https://github.com/user-attachments/assets/51897cdb-9fa8-47be-890d-d97d899cf69d" />
