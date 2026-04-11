# Practical Planner Qt

Aplicacion Qt Widgets para planificar trabajos practicos con login local, sesion recordada por 5 minutos, tablero en grilla, CRUD completo, editor de notas con guardado manual e historial visible con persistencia en archivo.

## Funcionalidades incluidas

- Login con usuarios locales guardados en `users.txt`.
- Registro de usuarios nuevos con guardado persistente en `users.txt`.
- Persistencia de sesion local en `session.txt` por 5 minutos.
- Tablero principal construido con `QGridLayout`.
- Filtros por estado y prioridad.
- Alta, edicion y eliminacion de trabajos practicos.
- Editor de notas asociado al trabajo practico seleccionado con guardado manual.
- Historial visible en pantalla y guardado en `history.txt`.
- Persistencia local de los trabajos practicos en `practicalworks.txt`.

## Usuarios de prueba

- `admin / admin123`
- `lucia / lucia123`
- `profe / profe123`

## Estructura del proyecto

```text
practical-planner-qt/
|-- practical-planner-qt.pro
|-- src/
|   |-- main.cpp
|   |-- planneroptions.h
|   |-- auth/
|   |   |-- authmanager.h
|   |   `-- authmanager.cpp
|   |-- models/
|   |   |-- user.h
|   |   |-- practicalwork.h
|   |   `-- historyentry.h
|   |-- storage/
|   |   |-- storagemanager.h
|   |   `-- storagemanager.cpp
|   |-- ui/
|   |   |-- logindialog.h
|   |   |-- logindialog.cpp
|   |   |-- registerdialog.h
|   |   |-- registerdialog.cpp
|   |   |-- taskdialog.h
|   |   |-- taskdialog.cpp
|   |   |-- mainwindow.h
|   |   `-- mainwindow.cpp
|   `-- resources/
|       |-- resources.qrc
|       `-- seed/
|           |-- users.txt
|           `-- practicalworks.txt
`-- README.md
```

## Como abrir y ejecutar en Qt Creator

1. Abre **Qt Creator**.
2. Ve a **File > Open File or Project...**.
3. Selecciona el archivo `practical-planner-qt.pro`.
4. Elige el kit **Desktop Qt 5.14.1 MinGW 64-bit** o el kit Qt Widgets equivalente que tengas instalado.
5. Presiona **Configure Project**.
6. Ejecuta con el boton **Run** o con `Ctrl+R`.

## Donde se guardan los archivos locales

La aplicacion crea y usa sus archivos locales en una carpeta `data` junto al ejecutable. Si corres desde Qt Creator, normalmente queda asi:

```text
practical-planner-qt/build/data
```

Archivos creados automaticamente:

- `users.txt`
- `practicalworks.txt`
- `history.txt`
- `session.txt`

## Flujo de uso

1. Inicia sesion con uno de los usuarios de prueba o crea uno nuevo con `Registrarse`.
2. Si vuelves a abrir la app dentro de 5 minutos en el mismo equipo, la sesion se restaura automaticamente.
3. Usa los filtros para ver trabajos por estado o prioridad.
4. Crea, edita o elimina trabajos desde el tablero. Cada cambio se guarda en `practicalworks.txt` y luego se recarga desde disco.
5. Abre las notas de un trabajo, edita el texto y guarda con el boton `Guardar notas`.
6. Revisa el historial a la derecha para ver las acciones realizadas.

## Verificacion realizada

El proyecto fue compilado correctamente en este entorno con:

- Qt 5.14.1
- MinGW 7.3.0
- `qmake`
- `mingw32-make`

El ejecutable generado quedo en:

```text
practical-planner-qt/build/practical-planner-qt.exe
```
