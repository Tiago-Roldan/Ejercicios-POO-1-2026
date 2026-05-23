# Ejercicio 004 - Tablero kanban estilo Trello (Grupal)

Aplicación de escritorio desarrollada en **Qt (C++)** con backend en FastAPI desplegado en un VPS, que permite gestionar tareas mediante un tablero estilo kanban.

El proyecto fue dividido en dos partes principales:

##Backend (en el VPS)

- Api de kanban desarrollada con FastAPI
- Base de datos MySQL
- Archivos Docker para compartimentalización del código

##Aplicacion de escritorio (App Qt Creator para el cliente)

- Interfaz gráfica de tablero estilo Kanban
- Comunicacion con la API del VPS
- Gestion de columnas y tarjetas de tareas

  
---


# Funcionalidades principales

##Backend

- C.R.U.D. de columnas y tarjetas
- Movimiento de tarjetas entre columnas y dentro de las mismas
- Persistencia en base de datos MySQL con tablas: "Columns", "Cards", y "Card_Order"


##Aplicacion QT

- Visualizacion del tablero Kanban
- C.R.U.D. de tarjetas y columnas
- Movimiento de tarjetas mediante botones en la interfaz
- Actualización tanto en tiempo real (Con polling cada 4 segundos) como manual (mediante un boton en la interfaz)


---

##Endpoints de la API (resumen)

El backend expone los siguientes endpoints principales:

- GET /board: Obtiene el estado completo del tablero, incluyendo columnas y tarjetas.
- POST /columns: Crea una nueva columna en el tablero.
- PUT /columns/{column_id}: Actualiza la información de una columna existente.
- DELETE /columns/{column_id}: Elimina una columna y sus tarjetas asociadas.
- POST /cards: Crea una nueva tarjeta dentro de una columna.
- PUT /cards/{card_id}: Modifica los datos de una tarjeta existente.
- DELETE /cards/{card_id}: Elimina una tarjeta del tablero.
- POST /cards/{card_id}/move: Mueve una tarjeta de una columna a otra.
- POST /cards/{card_id}/reorder: Cambia el orden de una tarjeta dentro de su columna.

---

##Formato JSON esperado de los Endpoints

- GET / board (Response)
```json
{
  "columns": [
    {
      "id": 1,
      "title": "To Do",
      "position": 0,
      "cards": [
        {
          "id": 10,
          "title": "Tarea",
          "description": "Descripción",
          "position": 0
        }
      ]
    }
  ]
}
```

---

- POST / columns (Request)
```json
{
  "title": "Nueva columna"
}
```

---

- POST / columns (Response)
```json
{
  "message": "created"
}
```

---

- PUT / columns / {column_id} (Request)
```json
{
  "title": "Nuevo título"
}
```

---

- PUT / columns / {column_id} (Response)
```json
{
  "message": "updated"
}
```

---

- DELETE / columns / {column_id} (Response)
```json
{
  "message": "deleted"
}
```

---

- Post / cards (Request)
```json
{
  "title": "Nueva tarjeta",
  "description": "Detalle",
  "column_id": 1
}
```

---

- Post / cards (Response)
```json
{
  "message": "created"
}
```

---

- PUT / cards / {card_id} (Request)
```json
{
  "title": "Nuevo título",
  "description": "Nuevo detalle",
  "column_id": 2
}
```

---

  - PUT / cards / {card_id} (Response)
```json
{
  "message": "updated"
}
```

---

- DELETE / cards / {card_id} (Response)
```json
{
  "message": "deleted"
}
```

---

  - POST / cards / {card_id} / move (Request)
```json
{
  "to_column_id": 2,
  "new_position": 0
}
```

---

  - POST / cards / {card_id} / move (Response)
```json
{
  "message": "moved"
}
```

---

  - POST / cards / {card_id} / reorder (Request)
```json
{
  "new_position": 1
}
```

---

  - POST / cards / {card_id} / reorder (Response)
```json
{
  "message": "reordered"
}
```

---

## Objetivo del ejercicio

Consolidar el uso de:

- API's REST
- Integracion Frontend y Backend
- Manejo de estado persistente
- Interfaces gráficas con Qt
- Sistemas colaborativos básicos
  
---

## Tecnologías usadas

- Frontend: Qt (desarrollado con C++) con asistencia de ChatGPT / Códex
- Backend: FastAPI (desarrollada con Python)
- Base de datos: MySQL
- Infraestructura: VPS de Contabo + Docker (compartimentalización)
