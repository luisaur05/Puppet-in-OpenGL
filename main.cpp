#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stack>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Camera.h"

// Tamaño de la ventana
const unsigned int WIDTH = 1500;
const unsigned int HEIGHT = 1500;

// Angulo de articulaciones
float angCabeza = 0.0f;
float angCabeza2 = 0.0f;
float angTorzo = 0.0f;
float angCadera = 0.0f;
float angHombro = 0.0f;
float angTobilloR = 0.0f;
float angTobilloL = 0.0f;
float angRodillaR = 0.0f;
float angRodillaL = 0.0f;
float angCaderaR = 0.0f;
float angCaderaL = 0.0f;
float angHombroR = 0.0f;
float angHombroL = 0.0f;
float angCodoR = 0.0f;
float angCodoL = 0.0f;
float angMunecaR = 0.0f;
float angMunecaL = 0.0f;

bool drawFigure = true;
bool reset = false;


// Vértices de un cubo
GLfloat vertices[] = {
    // Posiciones
    -0.5f, -0.5f, -0.5f, // 0
     0.5f, -0.5f, -0.5f, // 1
     0.5f,  0.5f, -0.5f, // 2
    -0.5f,  0.5f, -0.5f, // 3
    -0.5f, -0.5f,  0.5f, // 4
     0.5f, -0.5f,  0.5f, // 5
     0.5f,  0.5f,  0.5f, // 6
    -0.5f,  0.5f,  0.5f  // 7
};

// Índices para dibujar las caras del cubo
GLuint indices[] = {
    0, 1, 2, 2, 3, 0, // Cara trasera
    4, 5, 6, 6, 7, 4, // Cara delantera
    0, 1, 5, 5, 4, 0, // Cara inferior
    2, 3, 7, 7, 6, 2, // Cara superior
    0, 3, 7, 7, 4, 0, // Cara izquierda
    1, 2, 6, 6, 5, 1  // Cara derecha
};

// Shader de vértices
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 FragPos;
out vec3 Normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aPos; // Calcula la normal
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// Shader de fragmentos
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    // Ambiente
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Difusa
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Especular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

void createCube(GLuint& VAO, GLuint& VBO, GLuint& EBO) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

int main() {
    // Inicialización de GLFW
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Crear ventana
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Personaje Humanoide", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicialización de GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        return -1;
    }

    // Compilar shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Vincular los shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Liberar shaders compilados
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Crear cubo
    GLuint VBO, VAO, EBO;
    createCube(VAO, VBO, EBO);

    // Habilitar el z-buffer
    glEnable(GL_DEPTH_TEST);

    // Ciclo de renderizado

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        // Procesar entradas
        processInput(window);

        // Limpiar la pantalla
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Tell OpenGL a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // Usar el shader
        glUseProgram(shaderProgram);

        // Matrices de transformación
        glm::mat4 model = glm::mat4(1.0f);
        // Set up view and projection matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Enviar matrices a los shaders
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Configuración de la luz
        GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
        GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // Luz blanca
        glm::vec3 objectColor(0.5f, 0.3f, 0.2f); // Color marrón

        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 5.0f)));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(objectColor));


        // ImGUI window creation
        ImGui::Begin("Controles");
        // Text that appears in the window
        ImGui::Text("----Marioneta-----");
 
        ImGui::Checkbox("Reset", &reset);
        // Slider that appears in the window
        ImGui::SliderFloat("Torso", &angTorzo, -75.0f, 75.0f);
        ImGui::SliderFloat("Hombro", &angHombro, -65.0f, 65.0f);
        ImGui::SliderFloat("Cabeza", &angCabeza, -75.0f, 75.0f);
        ImGui::SliderFloat("Cabeza vert", &angCabeza2, -40.0f, 40.0f);
        ImGui::SliderFloat("Cadera", &angCadera, -65.0f, 65.0f);
        ImGui::SliderFloat("Pierna d", &angCaderaR, -65.0f, 65.0f);
        ImGui::SliderFloat("Pierna i", &angCaderaL, -65.0f, 65.0f);
        ImGui::SliderFloat("Rodilla d", &angRodillaR, 0.0f, 65.0f);
        ImGui::SliderFloat("Rodilla i", &angRodillaL, 0.0f, 65.0f);
        ImGui::SliderFloat("Tobillo d", &angTobilloR, -10.0f, 65.0f);
        ImGui::SliderFloat("Tobillo i", &angTobilloL, -10.0f, 65.0f);
        ImGui::SliderFloat("Brazo d", &angHombroR, -130.0f, 65.0f);
        ImGui::SliderFloat("Brazo i", &angHombroL, -130.0f, 65.0f);
        ImGui::SliderFloat("Antebrazo d", &angCodoR, 0.0f, 65.0f);
        ImGui::SliderFloat("Antebrazo i", &angCodoL, 0.0f, -65.0f);
        ImGui::SliderFloat("Mano d", &angMunecaR, -10.0f, 65.0f);
        ImGui::SliderFloat("Mano i", &angMunecaL, -10.0f, 65.0f);

   

        // Ends the window
        ImGui::End();

        // Renders the ImGUI elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Reinicia la marioneta
        if (reset) {
            angCabeza = 0.0f;
            angCabeza2 = 0.0f;
            angTorzo = 0.0f;
            angCadera = 0.0f;
            angTobilloR = 0.0f;
            angTobilloL = 0.0f;
            angRodillaR = 0.0f;
            angRodillaL = 0.0f;
            angCaderaR = 0.0f;
            angCaderaL = 0.0f;
            angHombro = 0.0f;
            angHombroL = 0.0f;
            angHombroR = 0.0f;
            angCodoR = 0.0f;
            angCodoL = 0.0f;
            angMunecaR = 0.0f;
            angMunecaL = 0.0f;
           
            reset = false;
        }
        // Matrices
        glm::mat4 Identidad(1.0f); /////// This
        glm::mat4 aux(1.0f); /////// This 

        std::stack<glm::mat4> Transformaciones; /////// This 

        Transformaciones.push(Identidad);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input de teclado para la cámara
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.processKeyboard('W', deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.processKeyboard('S', deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.processKeyboard('A', deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.processKeyboard('D', deltaTime);

        // Nuevas teclas para rotación
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.processKeyboard('Q', deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.processKeyboard('E', deltaTime);
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            camera.processKeyboard('R', deltaTime);
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
            camera.processKeyboard('F', deltaTime);


        // Actualización de la vista
        //glm::mat4 view = camera.getViewMatrix();
      
        // Torso
        glm::mat4 torso = glm::mat4(1.0f);
        torso = glm::rotate(torso, glm::radians(angTorzo), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotar torso
        Transformaciones.push(torso);
        torso = glm::scale(torso, glm::vec3(0.6f, 1.0f, 0.25f)); // Escalar torso
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(torso));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        // Hombros 
        glm::mat4 hombros = glm::translate(Transformaciones.top(), glm::vec3(0.0f, 0.5f, 0.0f)); // Trasladar hombros desde el torso
        hombros = glm::rotate(hombros, glm::radians(angHombro), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotar hombros
        Transformaciones.push(Transformaciones.top() * hombros);
        hombros = glm::scale(Transformaciones.top(), glm::vec3(1.05f, 0.2f, 0.5f)); // Escalar hombros
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(hombros));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        // Cuello (correctamente rotado y escalado)
        glm::mat4 cuello = glm::translate(Transformaciones.top(), glm::vec3(0.0f, 0.2f, 0.0f)); // Trasladar cuello desde el torso
        cuello = glm::rotate(cuello, glm::radians(angCabeza), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotar cabeza en eje X
        Transformaciones.push(cuello);
        cuello = glm::scale(Transformaciones.top(), glm::vec3(0.15f, 0.25f, 0.15f)); // Escalar cuello
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cuello));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Cabeza 
        glm::mat4 cabeza = glm::translate(Transformaciones.top(), glm::vec3(0.0f, 0.3f, 0.0f)); // Trasladar cabeza desde el torso
        cabeza = glm::rotate(cabeza, glm::radians(angCabeza2), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar cabeza en eje Y
        Transformaciones.push(cabeza);
        cabeza = glm::scale(Transformaciones.top(), glm::vec3(0.5f, 0.5f, 0.5f)); // Escalar cabeza
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cabeza));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        
        Transformaciones.pop(); //sacamos la cabeza
        Transformaciones.pop(); // sacamos el cuello

        // brazo derecho
        glm::mat4 brazoDerecho = glm::translate(Transformaciones.top(), glm::vec3(0.45f,-0.35f, 0.0f)); // Trasladar brazo al hombro
        brazoDerecho = glm::translate(brazoDerecho, glm::vec3(0.0f, 0.15f, 0.0f));
        brazoDerecho = glm::rotate(brazoDerecho, glm::radians(angHombroR), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        brazoDerecho = glm::translate(brazoDerecho, glm::vec3(0.0f, -0.15f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(brazoDerecho);

        brazoDerecho = glm::scale(Transformaciones.top(), glm::vec3(0.15f, 0.5f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(brazoDerecho));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        
        // antebrazo derecho
        glm::mat4 antebrazoDer = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.4f, 0.0f)); // Trasladar brazo al hombro
        antebrazoDer = glm::translate(antebrazoDer, glm::vec3(0.0f, 0.2f, 0.0f));
        antebrazoDer = glm::rotate(antebrazoDer, glm::radians(angCodoR), glm::vec3(-1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        antebrazoDer = glm::translate(antebrazoDer, glm::vec3(0.0f, -0.2f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(antebrazoDer);

        antebrazoDer = glm::scale(Transformaciones.top(), glm::vec3(0.15f, 0.4f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(antebrazoDer));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // mano derecha
        glm::mat4 manoder = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.3f, 0.0f)); // Trasladar brazo al hombro
        manoder = glm::translate(manoder, glm::vec3(0.0f, 0.1f, 0.0f));
        manoder = glm::rotate(manoder, glm::radians(angMunecaR), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotar brazo en el eje X (hombro)
        manoder = glm::translate(manoder, glm::vec3(0.0f, -0.1f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(manoder);

        manoder = glm::scale(Transformaciones.top(), glm::vec3(0.15f, 0.25f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(manoder));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        Transformaciones.pop(); // sacar la mano
        Transformaciones.pop(); // sacar el antebrazo
        Transformaciones.pop(); // sacar el brazo


        // brazo izquierdo
        glm::mat4 brazoIzquierdo = glm::translate(Transformaciones.top(), glm::vec3(-0.45f, -0.35f, 0.0f)); // Trasladar brazo al hombro
        brazoIzquierdo = glm::translate(brazoIzquierdo, glm::vec3(0.0f, 0.15f, 0.0f));
        brazoIzquierdo = glm::rotate(brazoIzquierdo, glm::radians(angHombroL), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        brazoIzquierdo = glm::translate(brazoIzquierdo, glm::vec3(0.0f, -0.15f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(brazoIzquierdo);

        brazoIzquierdo = glm::scale(Transformaciones.top(), glm::vec3(0.15f, 0.5f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(brazoIzquierdo));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        // antebrazo izquierdo
        glm::mat4 antebrazoIzq = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.4f, 0.0f)); // Trasladar brazo al hombro
        antebrazoIzq = glm::translate(antebrazoIzq, glm::vec3(0.0f, 0.2f, 0.0f));
        antebrazoIzq = glm::rotate(antebrazoIzq, glm::radians(angCodoL), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        antebrazoIzq = glm::translate(antebrazoIzq, glm::vec3(0.0f, -0.2f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(antebrazoIzq);

        antebrazoIzq = glm::scale(Transformaciones.top(), glm::vec3(0.15f, 0.4f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(antebrazoIzq));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        
        // mano izquierda
        glm::mat4 manoizq = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.3f, 0.0f)); // Trasladar brazo al hombro
        manoizq = glm::translate(manoizq, glm::vec3(0.0f, 0.1f, 0.0f));
        manoizq = glm::rotate(manoizq, glm::radians(angMunecaL), glm::vec3(0.0f, 0.0f, -1.0f)); // Rotar brazo en el eje X (hombro)
        manoizq = glm::translate(manoizq, glm::vec3(0.0f, -0.1f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(manoizq);

        manoizq = glm::scale(Transformaciones.top(), glm::vec3(0.15f, 0.25f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(manoizq));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        Transformaciones.pop(); // sacar la mano
        Transformaciones.pop(); // sacar el antebrazo
        Transformaciones.pop(); // sacar el brazo
        Transformaciones.pop(); // sacar hombros

        
        // Cadera
        glm::mat4 cadera = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.6f, 0.0f)); // Trasladar hombros desde el torso
        cadera = glm::rotate(cadera, glm::radians(angCadera), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotar hombros
        Transformaciones.push(cadera);

        cadera = glm::scale(Transformaciones.top(), glm::vec3(0.6f, 0.2f, 0.5f)); // Escalar hombros
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cadera));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

       
        // pierna derecho
        glm::mat4 Piernader = glm::translate(Transformaciones.top(), glm::vec3(0.2f, -0.35f, 0.0f)); // Trasladar brazo al hombro
        Piernader = glm::translate(Piernader, glm::vec3(0.0f, 0.15f, 0.0f));
        Piernader = glm::rotate(Piernader, glm::radians(angCaderaR), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        Piernader = glm::translate(Piernader, glm::vec3(0.0f, -0.15f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(Piernader);

        Piernader = glm::scale(Transformaciones.top(), glm::vec3(0.2f, 0.5f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Piernader));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        // pantorrilla derecho
        glm::mat4 pantorrillaDer = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.5f, 0.0f)); // Trasladar brazo al hombro
        pantorrillaDer = glm::translate(pantorrillaDer, glm::vec3(0.0f, 0.2f, 0.0f));
        pantorrillaDer = glm::rotate(pantorrillaDer, glm::radians(angRodillaR), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        pantorrillaDer = glm::translate(pantorrillaDer, glm::vec3(0.0f, -0.2f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(pantorrillaDer);

        pantorrillaDer = glm::scale(Transformaciones.top(), glm::vec3(0.2f, 0.5f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pantorrillaDer));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // pie derecho
        glm::mat4 pieder = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.3f, 0.0f)); // Trasladar brazo al hombro
        pieder = glm::translate(pieder, glm::vec3(0.0f, 0.1f, 0.0f));
        pieder = glm::rotate(pieder, glm::radians(angTobilloR), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        pieder = glm::translate(pieder, glm::vec3(0.0f, -0.1f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(pieder);

        pieder = glm::scale(Transformaciones.top(), glm::vec3(0.2f, 0.15f, 0.28f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pieder));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        Transformaciones.pop(); // sacar la pie
        Transformaciones.pop(); // sacar el pantorrilla
        Transformaciones.pop(); // sacar el pierna

        // pierna izquierda
        glm::mat4 Piernaizq = glm::translate(Transformaciones.top(), glm::vec3(-0.2f, -0.35f, 0.0f)); // Trasladar brazo al hombro
        Piernaizq = glm::translate(Piernaizq, glm::vec3(0.0f, 0.15f, 0.0f));
        Piernaizq = glm::rotate(Piernaizq, glm::radians(angCaderaL), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        Piernaizq = glm::translate(Piernaizq, glm::vec3(0.0f, -0.15f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(Piernaizq);

        Piernaizq = glm::scale(Transformaciones.top(), glm::vec3(0.2f, 0.5f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Piernaizq));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

       
        // pantorrrilla izquierda
        glm::mat4 pantorrillaIzq = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.5f, 0.0f)); // Trasladar brazo al hombro
        pantorrillaIzq = glm::translate(pantorrillaIzq, glm::vec3(0.0f, 0.2f, 0.0f));
        pantorrillaIzq = glm::rotate(pantorrillaIzq, glm::radians(angRodillaL), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        pantorrillaIzq = glm::translate(pantorrillaIzq, glm::vec3(0.0f, -0.2f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(pantorrillaIzq);

        pantorrillaIzq = glm::scale(Transformaciones.top(), glm::vec3(0.2f, 0.5f, 0.25f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pantorrillaIzq));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    
        // pie izquierdo
        glm::mat4 pieizq = glm::translate(Transformaciones.top(), glm::vec3(0.0f, -0.3f, 0.0f)); // Trasladar brazo al hombro
        pieizq = glm::translate(pieizq, glm::vec3(0.0f, 0.1f, 0.0f));
        pieizq = glm::rotate(pieizq, glm::radians(angTobilloL), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar brazo en el eje X (hombro)
        pieizq = glm::translate(pieizq, glm::vec3(0.0f, -0.1f, 0.0f)); // Volver a trasladar el brazo
        Transformaciones.push(pieizq);

        pieizq = glm::scale(Transformaciones.top(), glm::vec3(0.2f, 0.15f, 0.28f)); // Escalar brazo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pieizq));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        Transformaciones.pop(); // sacar la pie
        Transformaciones.pop(); // sacar el pantorrilla
        Transformaciones.pop(); // sacar el pierna

        
        Transformaciones.pop(); // sacar cadera
        Transformaciones.pop(); // sacar el torso
        glBindVertexArray(0);

        // Intercambiar buffers y sondear eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpiar
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}