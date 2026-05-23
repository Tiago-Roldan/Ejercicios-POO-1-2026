# Ejercicio 02 - Panel de monitoreo VPS
En un Empty qmake Project, crear una aplicación Qt de escritorio profesional que funcione como panel de monitoreo de un server en la nube (su propio VPS).

Debe existir un endpoint en el server que devuelva datos de salud (uptime, carga, memoria, disco o estado general), y la aplicación debe presentar un panel claro y legible con esos datos.

El panel debe mostrar: 
- estado general (OK, alerta, caído)
- Métricas principales
- Último chequeo
- Un historial corto de eventos.

Debe haber widgets interactivos (por ejemplo: QSpinBox, QPushButton, QLineEdit, QLabel). 

La elección y el rol de cada widget debe estar justificada por el problema.

Inspirarse en un sistema de monitoreo profesional (por ejemplo: Grafana, Zabbix, Prometheus, Netdata o Datadog) para el diseño del panel.

Incluir al menos un control para refresco manual y otro para configurar un umbral o intervalo de chequeo.

___

Los archivos subidos cumplen con las condiciones mencionadas arriba

La aplicacion utiliza un endpoint HTTP expuesto por una API creada mediante FastAPI, y desplegada mediante Docker en el VPS. La API nos permite acceder a las métricas del servidor (Uso de memoria, de CPU, etc), construyendo un panel visual al estilo de herramientas profesionales de monitoreo como Grafana o Zabbix

___

# Objetivos principales:

- Visualizar el estado general de nuestro VPS (OK / ALERTA / CAÍDO)
- Mostrar métricas principales (Ultime, carga del sistema, uso de memoria y disco)
- Registrar el último chequeo realizado
- Mantener un historial de eventos
- Permitir interaccion del usuario gracias a controles configurables

___

# Endpoint

La aplicacion utiliza el endpoint: http://147.93.189.45:8000/health para conectarse al vps, esperando un formato de respuesta como el siguiente:

```json
{
  "status": "OK",
  "uptime": "3 days 04:12:55",
  "load": 0.72,
  "mem_used": 2048,
  "mem_total": 4096,
  "disk_used": 35,
  "disk_total": 80
}
```

___

# Arquitectura:

## Cliente

- Interfaz gráfica desarrollada con Qt
- Consumo de API mediante solicitudes HTTP
- Monitoreo del VPS
- Manejo de estado e historial

## Servidor (VPS)

- API desarrollada con FastAPI y contenerizada con Docker
- Endpoint /health para la devolucion de las métricas del VPS

__

# Backend (API en VPS)

La API fue desarrollada con **FastAPI** y desplegada en un contenedor Docker.

Incluye:

- Endpoint /health
- Obtención de métricas del sistema
- Formato JSON estandarizado
- Ejecución aislada en contenedor

Incluida en el repositorio hay una carpeta con los archivos del servidor (Dockerfile, código de la API, etc.)
