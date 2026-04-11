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

Crear al menos una clase propia que contenga la lógica del monitoreo (no todo en main).
