# Ejercicio 08 - Editor Multilenguaje

Resumen del proyecto
- Implementación de un editor de código de escritorio multilenguaje (C++, Python, Java) orientado a POO y diseño por herencia/polimorfismo.
- La aplicación está pensada para funcionar 100% offline y no depende de servicios remotos.

Requisitos clave (cumplidos)

- **Login inicial**: credenciales por defecto `admin:1234`. Tras 3 intentos fallidos se aplica un bloqueo temporal cuya duración se lee desde el archivo de configuración.
- **Clase base abstracta `Pantalla`**: define la interfaz común y declara funciones virtuales puras obligatorias que deben sobrescribir las derivadas:
  - `inicializarUI()`
  - `conectarEventos()`
  - `cargarDatos()`
  - `validarEstado()`
  - `registrarEvento()`
- **Clases derivadas**: `Login`, `EditorPrincipal`, `ModoBloqueado`. El flujo de la aplicación trabaja siempre contra punteros o referencias a `Pantalla` (polimorfismo obligatorio).
- **Validadores de sintaxis**: jerarquía polimórfica con `ValidadorSintaxis` (abstracta) y derivadas `ValidadorCpp`, `ValidadorPython` y `ValidadorJava`.
  - La validación se realiza por línea: al abandonar la línea en edición (evento de `focusOutEvent` o al mover cursor), se ejecuta la verificación.
  - Si la línea es inválida, se resalta en rojo y se muestra un mensaje de diagnóstico amigable en la UI.

Eventos y redefinición consciente

- Se capturan y redefinen eventos en las clases derivadas según su responsabilidad:
  - `keyPressEvent`: atajos del editor (ej.: guardar, comentar línea, buscar, cambiar lenguaje).
  - `mousePressEvent`: interacción del cursor y selección de texto.
  - `resizeEvent`: adaptación visual del contenido y reflujo del área de código.
  - `closeEvent`: confirmación de salida y guardado automático/consulta al usuario.
  - `focusInEvent` / `focusOutEvent`: control de edición y disparo de validación por línea.

Registro de eventos y logging

- Todas las acciones relevantes (login, intentos fallidos, bloqueo, apertura/cierre de pantallas, exportación, errores de validación, atajos usados) se registran en un archivo de log local. Cada entrada incluye fecha/hora ISO y descripción.

Configuración y persistencia

- Se lee un archivo de configuración local (ej.: `config.ini`) con al menos las siguientes claves:

```
# config.ini (ejemplo de claves esperadas)
usuario_inicial=admin
contrasena_inicial=1234
tiempo_bloqueo_segundos=60
lenguaje_por_defecto=Cpp
ruta_exportacion=export/
```

- El tiempo de bloqueo, credenciales iniciales, lenguaje por defecto y ruta de exportación se obtienen desde este archivo.

Signals y slots

- La comunicación interna y notificación de cambios se implementa usando `signals/slots` tal como se vio en clase. Ejemplos de señales:
  - `senalLineaEditada(lineNumber)` -> conectado a validadores.
  - `senalArchivoGuardado(path)` -> conectado a logger y UI de notificación.

Comportamiento de flujo

- Al iniciar, se muestra la pantalla `Login` (derivada de `Pantalla`).
- Después de validar credenciales correctamente, se abre `EditorPrincipal` en modo full screen.
- Si se alcanzan 3 intentos fallidos, se instancia `ModoBloqueado` y se muestra el tiempo restante de bloqueo; la duración se lee desde `config.ini`.

Editor principal

- Selector de lenguaje con opciones: `C++`, `Python`, `Java`. El selector cambia el `ValidadorSintaxis` activo y el resaltado (tema) del editor.
- Validación por línea: al abandonar la línea (evento) se valida y, si hay error, se pinta la línea en rojo y se muestra diagnóstico.

Exportación final

- La aplicación incluye una funcionalidad para exportar todo el contenido del editor a un único archivo JPG que contenga el código completo, respetando saltos de línea y legibilidad (márgenes y tamaño de fuente adecuados). El archivo resultante se guarda en `ruta_exportacion` configurada.

Panel lateral - Currículum (no vacío)

- Para que la ventana principal no quede vacía, se incorpora un panel lateral con estilo tipo LinkedIn que contiene:
  - foto de perfil (opcional en configuración, por defecto espacio reservado sin imágenes),
  - descripción breve (extracto profesional),
  - lista de habilidades (p. ej. C++, Python, Diseño OO, UML),
  - información de contacto (email, LinkedIn URL).

Soporte offline

- Todo el funcionamiento es local: lectura/escritura de ficheros, validadores y exportación se realizan sin conexión a internet ni dependencias de servicios remotos.

Buenas prácticas y diseño

- Uso obligatorio de polimorfismo: el código cliente usa punteros/referencias a `Pantalla` y `ValidadorSintaxis` para invocar operaciones sin conocer la implementación concreta.
- `Pantalla` obliga la implementación de las funciones virtuales puras en cada derivada para garantizar comportamiento específico por pantalla.
- Reducción de acoplamiento mediante señales/slots y responsabilidades separadas (UI vs validación vs logging vs exportación).

Archivos relevantes

- El proyecto contiene (entre otros) las siguientes clases/archivos fuente relevantes a la especificación:
  - `pantalla.h` / `pantalla.cpp` (interfaz `Pantalla` y helpers comunes)
  - `login.h` / `login.cpp` (`Login` derivada)
  - `editorprincipal.h` / `editorprincipal.cpp` (`EditorPrincipal` derivada)
  - `modobloqueado.h` / `modobloqueado.cpp` (`ModoBloqueado` derivada)
  - `validadorsintaxis.h` / `validadorsintaxis.cpp` (jerarquía de validadores)
  - `logger.h` / `logger.cpp` (registro en archivo con timestamps)
  - `config.ini` (archivo de configuración local)

Notas de implementación y recomendaciones

- Validación por línea: conectar la señal de `focusOutEvent` del componente de edición para emitir `senalLineaEditada(lineNumber, text)` y que el `Validador` la procese inmediatamente.
- Para el resaltado y diagnóstico: mantener estructura que asocie número de línea -> estado (válido/ inválido) y mensaje; repintar solo las líneas afectadas.
- Exportación a JPG: renderizar el contenido formateado en un lienzo de mapa de bits (ej.: QPixmap/QImage en Qt), con tipografía monoespaciada y ajuste de ancho, luego guardar como JPEG de alta calidad.
- Logging: usar formato ISO 8601 para timestamps y rotación simple por tamaño o por día si el log crece.

Pruebas manuales sugeridas

1. Verificar login con `admin:1234` y que abra `EditorPrincipal` en full screen.
2. Intentar 3 logins fallidos y confirmar entrada a `ModoBloqueado` con contador del tiempo de bloqueo.
3. Seleccionar cada lenguaje y escribir líneas con errores sintácticos; al abandonar la línea debe mostrarse el resaltado en rojo y un diagnóstico legible.
4. Probar atajos (guardar, exportar) y comprobar entradas en el `logger`.
5. Exportar a JPG y abrir el archivo resultante para verificar legibilidad y saltos de línea.

Contacto y licencia

- Proyecto de práctica docente. Código y assets incluidos para uso educativo.

---

Este `README.md` enfatiza los requisitos pedidos en el enunciado: clases abstractas y virtuales puras, polimorfismo por punteros/referencias, validación de sintaxis por línea con resaltado y diagnóstico, captura y redefinición de eventos, logging con fecha, lectura desde `config.ini`, signals/slots, exportación a JPG y panel lateral con currículum.
