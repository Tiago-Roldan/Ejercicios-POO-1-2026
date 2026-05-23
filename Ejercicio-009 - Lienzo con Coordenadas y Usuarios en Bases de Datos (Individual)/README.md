# Ejercicio 09 - Coordenadas en Base de Datos

## Descripción General

Aplicación desarrollada en Qt que integra un sistema de dibujo a mano alzada con persistencia de datos en base de datos SQLite. Incluye autenticación de usuarios mediante login, registro de intentos de acceso y almacenamiento de coordenadas de trazo para reconstrucción posterior.

## Requisitos Funcionales Implementados

### Sistema de Dibujo (Clase Pintura)

- Clase `Pintura` derivada de `QWidget` que permite dibujo a mano alzada mediante movimiento del mouse.
- Captura de eventos de mouse para registrar el trazo en tiempo real.
- Almacenamiento de coordenadas en memoria durante la sesión.

### Controles del Pincel

- **Ajuste de grosor**: Rueda del mouse incrementa o decrementa el grosor del pincel.
- **Cambio de color**:
  - Tecla `R`: Color rojo
  - Tecla `G`: Color verde
  - Tecla `B`: Color azul
- **Limpiar lienzo**: Tecla `Escape` borra completamente el lienzo.
- **Deshacer**: Combinación `Ctrl+Z` revierte hasta las últimas 10 acciones de trazo.

### Autenticación y Registro

- Interfaz de login construida mediante Qt Designer (`loginwidget.ui`).
- Validación de credenciales contra usuarios almacenados en base de datos SQLite.
- Interfaz de registro para nuevos usuarios (`registerwidget.ui`).
- Almacenamiento seguro de usuarios en base de datos.

### Sistema de Logs

- Registro de accesos exitosos en archivo de log.
- Registro de intentos fallidos de login.
- Las contraseñas no se almacenan en logs por razones de seguridad.
- Los logs proporcionan auditoría de intentos de acceso.

### Persistencia de Datos

- Almacenamiento de coordenadas de trazo en base de datos SQLite.
- Cada trazo se asocia al usuario que lo realizó.
- Reconstrucción del dibujo al reabrir la sesión del usuario.
- Identificación de trazo mediante ID único.

## Estructura del Proyecto

```
Proyecto9.pro                 Archivo de configuración del proyecto Qt
main.cpp                      Punto de entrada de la aplicación
mainwidget.h / mainwidget.cpp Ventana principal de la aplicación
loginwidget.h / loginwidget.cpp Lógica del widget de login
loginwidget.ui               Interfaz del login (Qt Designer)
registerwidget.h / registerwidget.cpp Lógica del widget de registro
registerwidget.ui            Interfaz del registro (Qt Designer)
pinturas.h / pinturas.cpp    Clase Pintura para dibujo a mano alzada
Makefile                     Archivo de compilación
build/                       Directorio de construcción
```

## Configuración y Compilación

### Abrir el Proyecto en Qt Creator

1. Abrir Qt Creator.
2. Ir a `Archivo > Abrir archivo o proyecto`.
3. Navegar a la carpeta del proyecto y seleccionar el archivo `Proyecto9.pro`.
4. Qt Creator cargará automáticamente la configuración del proyecto.

### Compilar el Proyecto

Dentro de Qt Creator:
1. Seleccionar el kit de compilación (Qt 6.11.0 MinGW 64-bit).
2. Presionar `Ctrl+B` o ir a `Compilar > Compilar proyecto`.
3. El archivo ejecutable se generará en el directorio `build/`.

### Ejecutar la Aplicación

1. Presionar `Ctrl+R` o ir a `Compilar > Ejecutar`.
2. Se abrirá la ventana de login.
3. Ingresar credenciales existentes o registrarse como nuevo usuario.

## Base de Datos

### Estructura SQLite

La aplicación utiliza SQLite para almacenar:
- Datos de usuarios (nombre de usuario).
- Coordenadas de trazo con referencia al usuario.
- Propiedades de trazo (grosor, color).

### Administración de Base de Datos

Se puede utilizar SQLiteStudio o herramienta equivalente para:
- Inspeccionar la estructura de tablas.
- Consultar datos de usuarios y trazos.
- Verificar logs de acceso.
- Realizar consultas SQL personalizadas.

### Pasos para Administrar la Base de Datos

1. Descargar e instalar SQLiteStudio.
2. Abrir la herramienta y crear conexión con el archivo de base de datos del proyecto.
3. Navegar por las tablas para inspeccionar datos.
4. Ejecutar consultas SQL si es necesario.

## Uso de la Aplicación

### Primer Inicio

1. Ejecutar la aplicación.
2. Si no existe usuario, seleccionar opción de registro.
3. Ingresar nombre de usuario y contraseña.
4. Confirmar contraseña.
5. Presionar botón de registro.

### Login

1. Ingresar nombre de usuario.
2. Ingresar contraseña.
3. Presionar botón login.
4. Si las credenciales son válidas, se abrirá la ventana de dibujo.
5. Si fallan, se registrará el intento en logs.

### Dibujar

1. Usar el mouse para dibujar en el lienzo.
2. Presionar teclas `R`, `G`, `B` para cambiar color.
3. Usar rueda del mouse para ajustar grosor del pincel.
4. Presionar `Ctrl+Z` para deshacer hasta 10 acciones.
5. Presionar `Escape` para limpiar todo el lienzo.

## Detalles Técnicos

### Clase Pintura

La clase `Pintura` implementa:
- Manejo de eventos `mousePressEvent`, `mouseMoveEvent`, `mouseReleaseEvent`.
- Manejo de eventos `wheelEvent` para ajuste de grosor.
- Manejo de eventos `keyPressEvent` para cambio de color y limpiar lienzo.
- Algoritmo de deshacer basado en pila con límite de 10 acciones.
- Renderizado de trazo en `paintEvent`.

### Integración con SQLite

- Uso de librería `QSqlDatabase` y `QSqlQuery` para operaciones de base de datos.
- Validación de credenciales mediante consulta parametrizada.
- Almacenamiento de coordenadas mediante inserciones en tabla de trazo.
- Recuperación de trazo al abrir sesión del usuario.

### Sistema de Logs

- Logs almacenados en archivo de texto.
- Formato: timestamp, nombre de usuario, resultado (exitoso/fallido), dirección IP opcional.
- Logs sin información sensible (contraseñas).

## Consideraciones de Seguridad

- Las contraseñas se validan directamente contra la base de datos.
- No se guardan contraseñas en logs.
- Cada intento de login se registra con timestamp.
- Se mantiene auditoría de accesos para detectar intentos no autorizados.

## Herramientas Utilizadas

- Qt Framework 6.11.0
- Qt Creator (IDE)
- SQLite (Motor de base de datos)
- MinGW (Compilador)
- Qt Designer (Construcción de interfaces)

## Notas Adicionales

- El proyecto utiliza Makefiles generados automáticamente por qmake.
- Los archivos `.ui` de Qt Designer se convierten automáticamente en código C++ durante la compilación.
- Los archivos `moc_*.cpp` se generan automáticamente por el Meta-Object Compiler de Qt.
- El directorio `build/` contiene artefactos de compilación y puede eliminarse sin afectar el proyecto.

## Requisitos de Sistema

- Qt 6.11.0 o superior
- MinGW 64-bit (compilador)
- SQLite 3 (incluido en Qt)
- SO Windows (compilado con Makefile de MinGW)
