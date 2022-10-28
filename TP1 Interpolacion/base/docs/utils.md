## Resumen

* **Window**
  * `Window`: clase para simplificar la creación/destrucción una ventana glfw (incluyendo la incialización/cierre de glfw y glad, y opcionalmente lo necesario para usar imgui en la ventana).
  * `FrameTimer`: clase auxiliar para medir el tiempo entre frames y estimar el framerate.
* **Geometry**
  * `Geometry`:  clase para representar una malla en memoria, en un formato listo para enviar a la GPU.
  * `GeometryRenderer`:  clase para enviar una malla a la GPU y gestionar los buffers que almacenan esos datos en la GPU.
* **ObjMesh**
  * Clase (`ObjMesh`) y funciones auxiliares (`readObjMesh`, `readObjMeshes`) para leer un modelo (malla y materiales) a partir de archivos en el formato .obj de Wavefront, y convertirlo al formato necesario para enviar a la GPU (`toGeometry`).
* **Texture**
  * Clase (`Texture`) para cargar una textura desde un archivo .png hacia la GPU, y gestionar el uso y ciclo de vida de la misma.
* **Material**
  * Struct (`Material`) para describir un material (componentes para el modelo de iluminación de *Phong* y nombre del archivo de textura si es necesario).
* **Shader**
  * Clase (`Shader`) para simplificar la carga (desde archivos fuente) y compilación de shaders, y gestionar su uso y ciclo de vida.
  * Funciones alternativas (`loadShader`  y `loadShaders`) para simplificar solamente la carga y compilación de Shaders.
* **Debug**
  * Funciones de preprocesador para mostrar mensajes de log (`cg_info`) y manejar errores (`cg_assert` y `cg_error`).
* **Misc**
  * Funciones simples que son utilizadas como auxiliares en algunos de los demás fuentes.



## Window

*Nota:* Aunque el nombre hace referencia a la ventana, se incluye aquí también el código necesario para incializar las bibliotecas *glfw* y *glad*, ya que habitualmente estas incializaciones están intercaladas con el código de creación de la ventana (`glfwInit` antes de crearla, `gladLoadGLLoader` luego de obtener el contexto de la primera ventana). De igual forma, se encargan de cerrar estas bibliotecas al destruir la última ventana.

### Clase `Window`

Esta clase representa un puntero a una ventana GLFW (es directamente convertible a `GLFWwindow`), pero incluye en su constructor todo lo necesario para incializar la ventana, y en el destructor lo necesario para destruirla. El contexto OpenGL se crea para la versión 3.3 en modo Core.



Además, al inicializar la primera ventana, el constructor se encarga de inicializar primero la biblioteca GLFW (`glfwInit()`), y al destruir la última ventana, el destructor se encarga de cerrarla (`glfwShutdown()`).

Por esto, un programa puede empezar simplemente declarando una instancia de Window, y esto ya se encargará de incializar GLFW y glad automáticamente, y se asegurará de cerrarlas todo al finalizar.

Una vez creada una ventana, el resto de las acciones (como definir callbacks, sensar eventos, intercambiar buffers, etc, todo menos su destrucción) se deberán realizar con las funciones de la biblioteca GLFW, pasando el la instancia de `Window` como 1er argumento (donde las funciones esperan un `GLFWwindow*`). Esto significa que un main como el siguiente es completamente válido:

```C++
int main() {
    Window window(800,600,"CG Demo"); // inicializa glfw, crea la ventana e inicializa glad
	// ...setear callbacks...
	do {
		glClear(GL_COLOR_BUFFER_BIT);
		// ...dibujar...
		glfwSwapBuffers(X);
		glfwPollEvents();
	} while(!glfwWindowShouldClose(window));
} // automaticamente se destruye la ventana y se cierra la biblioteca glfw
```



Opcionalmente, constructor y destructor se encargan de inicializar/cerrar un contexto ImGui dentro de la ventana, para poder mostrar con esta biblioteca cuadros de diálogos.  Si se habilita el uso de ImGui (pasandole true al argumento `enable_imgui` del constructor), los métodos `ImGuiDialog` y `ImGuiFrame` simplifican el código necesario en cada frame. Ejemplo:

```C++
    Window window(800,600,"CG Demo",true); // el último argumento(true) inicializa ImGUi
	// ...setear callbacks...
	do {
		...
        window.ImGuiDialog("Ejemplo",[&](){ // "Ejemplo" es el título de la ventana ImGui
            // ...definir los widgets con las funciones de ImGui...
			ImGui::Text("Hola Mundo");
		});
		...
	} while(...);
```



Por último, el constructor de `Window` tiene un último argumento opcional de tipo `GLFWwindow*`. Si se le pasa un `GLFWwindow*` u otro `Window`, la nueva ventana compartirá el contexto OpenGL con la que se le pasa. Esto implica que ambas ventanas podrán utilizar los mismos shaders, buffers, texturas, etc. De lo contrario, cada ventana tiene su propio contexto y solo puede ver sus propios recursos.



### Clase `FrameTimer`

Esta clase se encarga de medir el tiempo en cada frame, permitiendo consultar el tiempo transcurrido entre un frame y otro, y una estimación del *frame-rate* (frames por segundo). Para ello, se debe declarar una instancia justo antes de comenzar el loop principal de la aplicación, e invocar al método `newFrame()` en cada iteración. Este método retorna el tiempo transcurrido desde su última invocación, y (una vez por segundo) actualiza el *frame-rate* estimado. Ejemplo:.

```C++
	FrameTimer ftimer;
	do {
        double delta_t = ftimer.newFrame(); // tiempo entre frames en segundos
        int frame_per_second = ftimer.getFrameRate(); // FPS estimado
        ...
    }
```



### Función `ImGui::Combo`

La función ImGui::Combo original de la biblioteca ImGui no permite mostrar fácilmente un arreglo de cadenas de texto representado mediante un `std:::vector<std::string>`. En `Window.hpp` se define una nueva sobrecarga para `ImGui::Combo` para solucionar este problema.



## Geometry

* `Geometry`:  clase para representar una malla en memoria, en un formato listo para enviar a la GPU.
* `GeometryRenderer`:  clase para enviar una malla a la GPU y gestionar los buffers que almacenan esos datos en la GPU.

## ObjMesh

* Clase (`ObjMesh`) y funciones auxiliares (`readObjMesh`, `readObjMeshes`) para leer un modelo (malla y materiales) a partir de archivos en el formato .obj de Wavefront, y convertirlo al formato necesario para enviar a la GPU (`toGeometry`).

## Texture

* Clase (`Texture`) para cargar una textura desde un archivo .png hacia la GPU, y gestionar el uso y ciclo de vida de la misma.

## Material

* Struct (`Material`) para describir un material (componentes para el modelo de iluminación de *Phong* y nombre del archivo de textura si es necesario).

## Shader

* Clase (`Shader`) para simplificar la carga (desde archivos fuente) y compilación de shaders, y gestionar su uso y ciclo de vida.
* Funciones alternativas (`loadShader`  y `loadShaders`) para simplificar solamente la carga y compilación de Shaders.



## Debug

Estas macros permiten agregar controles y mensajes para utilizar en modo *Debug* y no tener que eliminar manualmente al finalizar el programa; en modo *Release* se desactivan automáticamente (no hace falta entonces eiminarlos del código).

### Macro `cg_assert`

Esta macro se utiliza en lugar de la función `assert` de C/C++. El objetivo es verificar que una condición se cumple, y detener el programa si no lo hace. Además de la condición a verificar, la macro también incluye un mensaje de diagnóstico. Se define en términos de alguna de las otras macros `cg_assert__XXX`, que son las posibles implementaciones reales. La opciones son invocar a la función `std::assert` de la biblioteca estándar, lanzar una excepción (`std::runtime_exception`), mostrar un mensaje y simular un punto de interrupción (esto hace la interrupción 3 en ensamblador, permite detenerse en el lugar del error durante la depuración), o simplemente no hacer nada. Usted puede modificar el fuente para elegir la implementación que prefiera, o hasta agregar una propia.

La idea detrás de esta macro es utilizarla para verificar cualquier condición que se supone cierta, y para la cual no se prevee curso de acción alternativo en caso de no serlo (es decir, debe ser cierta o el programa se cerrará). Este tipo de verificaciones permite detectar errores tempranamente. Por ej, si al crear una ventana no se habilita ImGui, pero luego sí se invoca a funciones de ImGui para intentar mostrar un widget, el programa seguramente revienta en algún paso dentro de la función de ImGui. Pero no es ahí donde está la cuasa del error, sino mucho antes. Verificar con esta macro que el contexto ImGui esté habilitado para la ventana antes de querer dibujar widgets haría que el error se detectara antes y de forma más clara.

Se recomienda abusar de estas verificaciones (escriba sin miedo todas las que se le ocurran), porque total al compilar en modo release (para eso está el `#ifdef NDEBUG`) estas verificaciones. Es decir que, si se quiere, la versión release no hace ninguna verificación. Por esto no debe preocuparle que muchas verificaciones puedan generar un probleama rendimiento. 

Finalmente, dado que usualmente en release las verificaciones no se realizan, la correcta ejecución del programa nunca debe depender de un *side-effect* de la condición. Por ej, algo como `cg_assert(x++!=0,"x debía ser 0")` hará que en release, no solamente no se verifique si `x` estaba en `0`, sino que tampoco se incremente. Si el incremento es importante, mejor usar `cg_error` de la siguiente forma: `if (x++!=0) cg_error("x debía ser 0");`.

### Macro `cg_error`

Detiene el programa informando un error. Por defecto se implementa a partir de `cg_assert`, por lo que la forma de informar y detenerse (si es que se detiene, podría no hacer nada) depende de la implementación de `cg_assert`. Por defecto, se detendrá al compilar y correr en modo *Debug* pero no en modo *Release*.

### Macro `cg_info`

En modo *Debug*, muestra un mensaje (por defecto en consola, puede modificar su implementación). En modo *Release* no hace nada. Es útil para mostrar información que ayuda al desarrollo, pero que ya no es necesaria una vez finalizado el mismo; ya que al pasar a modo release los mensajes se desactivan automáticamente (no hace falta eliminarlos del código).



## Misc

Aquí hay algunas funciones libres variadas. No fueron pensadas para ser consumidas por el usuario final de estas bibliotecas (aunque puede hacerlo si las encuentra útiles), sino que son utilizadas por los demás fuentes de *utils* y están aquí simplemente para que esos otros fuentes no deban repetir código.

