## Estructura de archivos y directorios

* `src`: Código fuente de cada uno de los proyectos/tps de la materia.
* `bin`: Directorio de trabajo a utilizar al ejecutar los programas compilados
  * `bin/shaders`: códigos fuentes de los shaders que el programa leerá en tiempo de ejecución
  * `bin/models`: modelos (mallas, materiales y texturas) que los programas pueden cargar (en formato .obj).
* `third`: Fuentes de bibiotecas de terceros
  * [ImGui](https://github.com/ocornut/imgui/): para mostrar widgets en un viewport OpenGL.
  * [stb_image](https://github.com/nothings/stb): para leer archivos de imagenes (texturas) de distintos formatos (como .jpg o .png).
  * [glad](https://github.com/Dav1dde/glad): para acceder a las extensiones/funcionalidades modernas de OpenGL.
* `docs`: Documentación varia principalmente relacionada al código fuente (arquitectura, estructura de archivos, ejemplos de uso, etc).
* `utils` Clases y funciones desarrollados específicamente para los proyectos de esta materia, con el objetivo de simplificar tareas complicadas y/o repetitivas.
* `tmp`: Directorio que se crea al compilar desde ZinjaI (o con los Makefiles), y contiene todos los archivos temporales y binarios generados por la compilación (se puede borrar completamente y recrear al recompilar).

