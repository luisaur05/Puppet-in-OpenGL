# 3D Puppet Project with Cubes in OpenGL

This project implements a 3D puppet made of cubes, with joint movements using forward kinematics. The scene includes a camera that can rotate around the figure, allowing observation of its movements from different angles.

## Requirements

To run the project, you need the following programs and libraries installed:

- **OpenGL**: For rendering 2D and 3D graphics.
- **GLFW**: For managing windows and user input.
- **GLEW**: For handling OpenGL extensions.
- **GLM**: For linear algebra operations (matrices and vectors) in graphics.
- **ImGui**: For creating graphical user interfaces.

Make sure the libraries are correctly linked in your development environment before compiling the project.

### Installation Instructions

1. **Install dependencies**:
   - On Windows, download the appropriate versions of GLFW, GLEW, GLM, and ImGui from their official websites and configure them in your development environment.

2. **Compile the project**:
   You can compile the entire project with the libraries using Visual Studio, or if you're using `g++`, run the following command:
   ```bash
   g++ main.cpp Camera.cpp -o Puppet3D -lglfw -lGLEW -lGL -DIMGUI_IMPL_OPENGL_LOADER_GLEW
3. **Run the project**:
   After compiling, execute the generated file to display the puppet on screen. The puppet can be moved using the controls, and the camera can also be adjusted for better observation using the mentioned controls.

## Controls

### Camera Movement:
- **W/A/S/D**: Move the camera around the puppet.
- **Q/E**: Rotate the camera counterclockwise and clockwise, respectively.
- **R/F**: Rotate the camera upward and downward.

### Joint Control:
A graphical interface implemented with **ImGui** allows you to adjust the angles of the puppet's main joints, such as shoulders, elbows, ankles, and others.

## Demo Video
[https://youtu.be/sySXCzEik4Y](https://youtu.be/sySXCzEik4Y)
