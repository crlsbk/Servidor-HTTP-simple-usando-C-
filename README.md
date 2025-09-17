# Servidor HTTP Simple usando C++

Este proyecto implementa un servidor HTTP básico escrito en C++. Permite servir archivos estáticos como HTML, CSS y JavaScript desde la carpeta `src/`.

## Características

- Servidor HTTP multicliente (maneja una conexión a la vez)
- Soporta archivos `.html`, `.css`, `.js`, `.png`, `.jpg`, `.jpeg` y texto plano
- Respuestas HTTP con los códigos apropiados (200, 400, 404, 405)
- Fácil de modificar y expandir

## Ejecución

Tras ejecutar el archivo C++ con el compilador de tu elección, el servidor escuchará en [http://localhost:8080](http://localhost:8080).

Abre tu navegador y visita esa URL para ver la página servida desde `src/index.html`.

## Notas

- Los archivos deben estar en la carpeta `src/`.
- El servidor solo implementa el método GET.
- Si el archivo solicitado no existe, responde con 404.
- Para detener el servidor, presiona `Ctrl+C` en la terminal.
