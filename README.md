# Proyecto Marioneta 3D con Cubos en OpenGL

Este proyecto consiste en la implementación de una marioneta 3D compuesta de cubos, con movimientos en sus articulaciones mediante el uso de cinemática directa. La escena incluye una cámara que permite rotar alrededor de la figura para observar sus movimientos desde diferentes ángulos.

## Requisitos

Para ejecutar el proyecto, necesitas tener instalados los siguientes programas y librerías:

- **OpenGL**: Renderizar gráficos 2D y 3D.
- **GLFW**: manejar ventanas y entradas del usuario.
- **GLEW**: Manejar extensiones de OpenGL.
- **GLM**: Operaciones de álgebra lineal (matrices y vectores) en gráficos.
- **ImGui**: interfaces graficas de usuario.

Hay que asegurarse  que las bibliotecas estén correctamente vinculadas en tu entorno de desarrollo antes de compilar el proyecto.

### Instrucciones de Instalación

1. **Instalar las dependencias**:
   - Se trabajo cn Windows  descargando las versiones de GLFW, GLEW, GLM e ImGui desde sus sitios oficiales y configurando las propiedades del  entorno de desarrollo.


2. **Compilar el proyecto**:
   Podemos compilar el proyecto completo con las librerias en Visual Studio o en el caso de que estés usando `g++`, el comando para compilar:
   ```bash
   g++ main.cpp Camera.cpp -o Marioneta3D -lglfw -lGLEW -lGL -DIMGUI_IMPL_OPENGL_LOADER_GLEW
3. **Ejecutar el proyecto**:
   Después de compilar se ejecuta el archivo generado y se muestra en pantalla la marioneta a la cual podemos mover con los controles y ademas controlar una camara para una mejro apresiacion con los controles ya mencionados.
 
 

## Controles

### Movimiento de la cámara:
- **W/A/S/D**: Mover la cámara alrededor de la marioneta.
- **Q/E**: Rotar la cámara en sentido antihorario y horario respectivamente.
- **R/F**: Rotar la cámara hacia arriba y abajo.

### Control de las articulaciones:
Se implementa una interfaz gráfica mediante **ImGui** que permite ajustar los ángulos de las articulaciones principales de la marioneta como hombros, codos, tobillos, entre otros.




Demo video: https://youtu.be/sySXCzEik4Y
