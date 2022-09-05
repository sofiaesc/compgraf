## Compilación

### Pre-requisitos

Para compilar estos proyectos se requiere de un compilador C++ con soporte para C++14 o superior, y un conjunto de bibliotecas adiconales (que no son parte del estándar C++ y por ende deben instalarse por separado). Las bibliotecas adicionales son: *OpenGL*, *glfw* y *glm*[^third].

En *GNU/Linux* estas bibliotecas deben instalarse con el gestor de paquetes que corresponda a la distribución (por ej, con `apt-get` en *Ubuntu* y derivados, o con `dnf` en *Fedora*). Notar que se deben instalar las versiones para desarrollo (paquetes que usualmente finalizan con `-dev` o `-devel`).

En Windows, si utiliza *ZinjaI* en *Microsoft Windows* estas bibliotecas pueden instalarse simplemente intalando el complemento para *ZinjaI* porvisto por la cátedra. Si utiliza otro IDE o compilador, deberá instalarlas por su cuenta.

[^third]: También se utilizan las biblioteca *glad*, *stb_image* y *Dear ImGui*, pero los fuentes de estas bibliotecas ya están incluidos. 

### Compilación y Ejecución

#### ZinjaI

En *Microsoft Windows*  o en *GNU/Linux* se puede utilizar el IDE [*ZinjaI*](zinjai.sourceforge.net/) para explorar, editar, compilar y ejecutar estos proyectos. Solo necesita agregar a ZinjaI el complemento proporcionado por la cátedra. Para instalar el complemento, debe ir al menú *Herramientas*, seleccionar *Instalar complementos...*, y seguir las instrucciones; o puede directamente arrastrar el archivo del complemento (.zcp) a la ventana de *ZinjaI*.

Se debe instalar el complemento adecuado de acuerdo al sistema operativo, y la versión de *MinGW* que utilice en el caso de *Windows* (32bits o 64bits, siendo 32 el predeterminado, aún en sistemas de 64bits). Para evitar problemas de diferencias entre versiones (especialemente en sistemas *Windows*) se recomienda actualizar *ZinjaI* antes de instalar el complemento.

#### Makefiles

Cada proyecto cuenta con un archivo Makefile con las reglas de compilación del mismo. Ejecutando la herramienta *make* desde la carpeta con el código fuente del proyecto se puede compilar el mismo. El resultado se ubicará en alguna subcarpeta dentro de `tmp`, pero la ejecución debe lanzarse desde la carpeta `bin` para que el ejecutable encuentre los archivos necesarios en tiempo de ejecución (como modelos y shaders).

#### Otros

Si utiliza otro IDE u otra herramienta para gestionar la compilación, debe tener en cuenta lo siguiente:

* La versión del estándar C++ utilizada debe ser C++14 o superior.
* Agregar los directorios `utils`, `third/imgui`, `third/stb` e `third/glad` a la lista de directorios donde buscar headers.
* Utilizar las opciones de compilación y enlazado necesarias para las bibliotecas externas:
  * En *GNU/Linux*: *gl*, *glfw*, *glm* y *dl*.
  * En *Microsoft Windows*: *glfw3*, *opengl32* y *gdi32*.
  * En macOS:  *glfw*, *Cocoa*, *OpenGL*, *IOKit* y *glm*.
* Definir la constante de preprocesador `GLFW_INCLUDE_NONE` y, en caso de compilar en modo *release*,  también `NDEBUG`.
* Se deben compilar y enlazar los siguientes fuentes:
  * `src/*.cpp`
  * `utils/*.cpp`
  * `third/glad/glad.c`
  * `third/stb/stb_image.c`
  * `third/imgui/*.cpp`
  * `third/imgui/backends/imgui_impl_glfw.cpp`
  * `third/imgui/backends/imgui_impl_opengl.cpp`

El ejecutable resultante debe ser invocado desde el directorio `bin` para que encuentre los archivos necesarios en tiempo de ejecución (como modelos y shaders).



