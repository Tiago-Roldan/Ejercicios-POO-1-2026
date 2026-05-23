# Login con Clima y CV

Aplicación Qt de escritorio que integra un sistema de autenticación seguro con consumo de APIs externas, descarga de recursos remotos y una interfaz elegante con información climática en tiempo real.

## Descripción General

Esta es una aplicación desarrollada en C++ con Qt Framework que implementa un flujo de login seguro con validaciones, consumo de API de clima, descarga de imágenes de fondo desde URLs públicas y presentación de un currículum profesional. La aplicación demuestra conceptos avanzados de programación orientada a objetos, manejo de redes y arquitectura de software.

## Características Principales

### Autenticación y Seguridad

- **Login con QtDesigner**: Interfaz de usuario intuitiva diseñada con Qt Designer
- **Credenciales**: Usuario `admin` con contraseña `1234`
- **Bloqueo temporal**: Tras 3 intentos fallidos, la aplicación bloquea temporalmente el acceso
- **Validación robusta**: Verificación de credenciales antes de permitir acceso

### Consumo de API de Clima

- **Integración de API externa**: Consumo directo de API de clima para obtener datos de Córdoba
- **Autenticación simple**: API key en header o query parameter
- **Consulta sin SDKs**: Implementación directa sin uso de SDKs oficiales
- **Datos en tiempo real**: Temperatura y hora local mostradas en la pantalla de login
- **QNetworkAccessManager**: Uso obligatorio de la clase de Qt con connects tal como se enseñó en clase

### Descarga de Recursos

- **Descarga de imagen de fondo**: Obtención de imagen desde URL pública
- **Cache local**: Almacenamiento local de recursos descargados para optimizar rendimiento
- **Soporte para proxy**: Configuración de proxy para acceso a través de redes corporativas
- **Ventana full screen**: La ventana principal se abre en pantalla completa solo cuando la descarga de imagen termina

### Arquitectura y Diseño

- **Clase base abstracta**: `Pantalla` proporciona interfaz común para todas las ventanas
- **Herencia**: Dos clases derivadas: `Login` y `Ventana` heredan de `Pantalla`
- **Clase especializada**: `Clima` para manejo centralizado de datos meteorológicos
- **Signals/Slots**: Uso extensivo de mecanismo signal/slot tal como se vio en clase
- **Imagen con aspecto preservado**: Imagen principal se ajusta a la ventana manteniendo relación de aspecto sin deformación

### Configuración y Persistencia

- **Archivo de configuración**: Lectura desde archivo `config.ini`
- **Parámetros configurables**: API key y ciudad desde archivo externo
- **Logger integrado**: Registro de todos los eventos en archivo de log con fecha y descripción

### Confiabilidad y Robustez

- **Validación de datos**: Verificación exhaustiva de datos recibidos de APIs externas
- **Mensajes de error amigables**: Notificaciones claras en la interfaz para el usuario
- **Modo offline**: Soporte para funcionamiento sin conexión a internet con datos simulados
- **Detección de conexión**: Control automático de disponibilidad de red
- **Manejo de errores de red**: Recuperación elegante ante fallos de conexión

### Interfaz de Usuario

- **Ventana principal profesional**: Presentación elegante y limpia
- **Currículum integrado**: Sección al estilo LinkedIn con:
  - Foto de perfil profesional
  - Descripción breve del desarrollador
  - Información presentada de manera elegante y profesional

## Estructura del Proyecto

```
Codigo/
├── main.cpp                  # Punto de entrada de la aplicación
├── appcontroller.h/cpp       # Controlador principal de la aplicación
├── appconfig.h               # Configuración global de la app
├── pantalla.h/cpp            # Clase base abstracta para ventanas
├── login.h/cpp               # Ventana de login (derivada de Pantalla)
├── login.ui                  # Interfaz de login diseñada con Qt Designer
├── ventana.h/cpp             # Ventana principal (derivada de Pantalla)
├── ventana.ui                # Interfaz de ventana principal
├── clima.h/cpp               # Clase para manejo de datos climáticos
├── backgrounddownloader.h/cpp # Gestor de descarga de imágenes de fondo
├── configmanager.h/cpp       # Gestor de configuración (config.ini)
├── logger.h/cpp              # Sistema de logging con fechas
├── LoginClimaCV.pro          # Archivo de proyecto Qt
├── config.ini                # Archivo de configuración (API key, ciudad)
└── cache/                    # Directorio para almacenamiento en caché
```

## Requisitos Técnicos Implementados

### Obligatorios

- [x] QtDesigner para diseño de interfaz de login
- [x] Credenciales: admin/1234
- [x] Bloqueo temporal tras 3 intentos fallidos
- [x] Consumo de API de clima (Córdoba) con autenticación
- [x] Consulta directa sin SDKs oficiales
- [x] Temperatura y hora local en login
- [x] Descarga de imagen de fondo desde URL pública
- [x] Cache local de recursos
- [x] Soporte para proxy
- [x] QNetworkAccessManager con connects
- [x] Ventana principal full screen tras descarga completada
- [x] Imagen ajustada sin deformación, preservando aspecto
- [x] Clase base abstracta Pantalla
- [x] Clases derivadas Login y Ventana
- [x] Clase adicional Clima
- [x] Logging con fecha y descripción en archivo
- [x] Lectura de configuración desde archivo
- [x] Validación de datos y mensajes de error
- [x] Modo offline con datos simulados
- [x] Detección de conexión
- [x] Signals/Slots
- [x] Currículum al estilo LinkedIn en ventana principal

## Archivos de Configuración

### config.ini

Archivo de configuración que contiene:

```ini
[API]
key=YOUR_API_KEY
city=Cordoba

[PROXY]
enabled=false
host=
port=8080

[CACHE]
directory=./cache

[UI]
fullscreen=true
```

## Sistema de Logging

Los eventos se registran automáticamente en archivo de log con:
- Fecha y hora exacta
- Tipo de evento
- Descripción detallada
- Información de error (cuando aplique)

Ejemplos de eventos registrados:
- Intentos de login
- Bloqueos temporales
- Descargas de recursos
- Errores de conexión
- Éxitos de operaciones

## Requisitos del Sistema

- Qt 5.12 o superior
- C++ 17
- Compilador compatible (MSVC, GCC, Clang)
- Conexión a internet (para modo online)

## Dependencias Qt

- Core
- Gui
- Widgets
- Network (para consumo de APIs y descargas)

## Compilación

```bash
qmake LoginClimaCV.pro
make
```

## Ejecución

```bash
./LoginClimaCV
```

## Flujo de la Aplicación

1. Inicio de la aplicación
2. Presentación de pantalla de login
3. Muestra de temperatura actual en Córdoba
4. Validación de credenciales (máximo 3 intentos)
5. Descarga de imagen de fondo desde URL
6. Apertura de ventana principal en full screen
7. Presentación de currículum profesional

## Manejo de Errores

La aplicación está preparada para:
- Fallos de conexión a Internet
- Timeouts en descargas
- Respuestas inválidas de API
- Ficheros de configuración faltantes
- Imágenes corruptas o inválidas

En todos los casos, la aplicación notifica al usuario de manera amigable y continúa funcionando (en modo offline si es necesario).

## Notas de Desarrollo

- Todo uso de red se implementa mediante QNetworkAccessManager
- Todos los connects signal/slot siguen el patrón visto en clase
- La arquitectura modular permite fácil extensión y mantenimiento
- El sistema de caché reduce la carga de red y mejora la experiencia del usuario
- La validación exhaustiva de datos garantiza seguridad y estabilidad

## Licencia

Proyecto educativo - Ejercicio 07 POO 2026
