# Ejercicio 03 - Lienzo colaborativo en tiempo real

Aplicación de escritorio desarrollada en **Qt (C++)** con backend en **FastAPI desplegado en un VPS**, que permite dibujar de forma colaborativa en tiempo real sobre un lienzo con trazos suaves y continuos.

---

## Características principales

- Dibujo libre a mano alzada
- Sincronización colaborativa entre múltiples usuarios
- Persistencia del dibujo en servidor VPS
- Trazos suaves, continuos e independientes de la velocidad del mouse
- Interpolación dinámica de colores
- Control de grosor en tiempo real
- Herramienta de borrado integrada

---

## Tecnologías utilizadas

- **Frontend:** Qt (C++)
- **Backend:** FastAPI (Python)
- **Servidor:** VPS con Docker
- **Comunicación:** HTTP (API REST)
- **Asistencia de desarrollo:** GPT 5.2 Codex

---

## Controles de usuario

### Dibujo

- **Click izquierdo (mantener presionado):** Dibujar
- **Click derecho (mantener presionado):** Borrar (goma)

### Color

- Teclas **1 a 9** → Selección de color
- Se aplica una interpolación progresiva entre:
  - RGB inicial: `(192, 19, 76)`
  - RGB final: `(24, 233, 199)`

### Grosor

- **Rueda del mouse (scroll):** Ajusta el grosor del trazo
- Aplica tanto para lápiz como para goma

---

## Lógica de dibujo

El sistema implementa:

- Interpolación de puntos
- Suavizado de trazos
- Renderizado continuo sin segmentos visibles
- Independencia de la velocidad del mouse

---

## Arquitectura del sistema

El proyecto sigue una arquitectura modular:

- **Modelo (`ModeloDibujo`)**
  - Almacena los trazos del lienzo
  - Representa el estado del dibujo

- **Vista (`VistaLienzo`)**
  - Renderiza el dibujo usando `paintEvent`
  - Maneja la interacción del usuario

- **Sincronización (`GestorSincronizacion`)**
  - Comunicación con la API en el VPS
  - Manejo de envío y recepción de datos

---

## Backend (FastAPI)

El servidor VPS expone endpoints para:

- Obtener el estado actual del dibujo
- Guardar el estado del dibujo

### Formato esperado del JSON

```json
{
  "strokes": [ {
    "id": "d2b1b0f1-8f80-4d2d-8bd3-8b5e0a6d7f22",
    "width": 6,
    "eraser": false,
    "color": { "r": 192, "g": 19, "b": 76, "a": 255 },
    "points": [ { "x": 120.5, "y": 200.0 }, { "x": 121.2, "y": 201.1 } ]
  } ]
}
```

---

## Persistencia

- Al iniciar la aplicación:
  - Se recupera automáticamente el dibujo almacenado en el servidor
- Al presionar **Guardar**:
  - Se envía el estado actual del lienzo al VPS

---

## Colaboración en tiempo real

- El dibujo es compartido entre todos los usuarios
- Estrategia implementada:
  - Modelo incremental de dibujo
  - Fusión de trazos (merge)

### Reglas:

- Cuando un usuario guarda:
  - Se actualiza el estado en el servidor
- Los demás usuarios:
  - Pueden ver los cambios
  - Sin perder lo que están dibujando localmente

---

## Consideraciones importantes

- No se pierde información al guardar
- El sistema maneja conflictos mediante fusión de trazos
- La experiencia es fluida incluso con múltiples usuarios

---

## Ejecución

### Backend (FastAPI)

- Levantar el archivo docker-compose.yml de la parte de Servidor del proyecto, posterior a haberlo cargado en un VPS. (Aún así, el proyecto ya funciona con conexión a nuestro propio VPS).

### Frontend (Qt)

- Abrir el proyecto en Qt Creator
- Compilar y ejecutar

---

## Integrantes del equipo de desarrollo

- Bianconi Clara  
- Guzmán Domínguez Lucas Santiago  
- Roldan Tiago Ignacio  

