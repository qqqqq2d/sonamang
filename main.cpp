#include <glad/gl.h>
#include <stdio.h>
#include </home/mattias/prog/sona_mang/glfw-3.4/include/GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

//Sõna mängu osa

std::vector<std::string> kombinatsioonid{};

//Lugeda kombinatsioonid listist
std::string kombinatsioon;
std::ifstream kombinatsioonide_list("kombinatsioonid.txt");

std::string sõnavahetus() {

	while (std::getline (kombinatsioonide_list, kombinatsioon)) {
		kombinatsioonid.push_back(kombinatsioon);
	}

	int suvaline_kombinatsioon;
	std::random_device dev;
	std::mt19937 rng(dev());

	std::uniform_int_distribution<std::mt19937::result_type> kombinatsiooni_vahemik(0,kombinatsioonid.size()-1);
	
	suvaline_kombinatsioon = kombinatsiooni_vahemik(rng);
	
	std::string rnames = kombinatsioonid[suvaline_kombinatsioon];
	
	//kombinatsioonid.erase(std::remove(kombinatsioonid.begin(), kombinatsioonid.end(), rnames), kombinatsioonid.end());
	
	return kombinatsioonid[suvaline_kombinatsioon];
}

// Camera class
class Camera {
public:
    glm::vec3 position = glm::vec3(-2.0f, 0.0f, 8.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 45.0f;

    void updateCameraVectors() {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }
};

// Global variables
Camera camera;
bool firstMouse = true;
float lastX = 400, lastY = 300;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Structure to hold character data
struct Character {
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

// Mouse callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    if (camera.pitch > 89.0f) camera.pitch = 89.0f;
    if (camera.pitch < -89.0f) camera.pitch = -89.0f;

    camera.updateCameraVectors();
}

// Process keyboard input

std::string text = sõnavahetus();
bool SPACE_PRESSED = false;

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += cameraSpeed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= cameraSpeed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (!SPACE_PRESSED)) {
    	SPACE_PRESSED = true;
    	text = sõnavahetus();
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
    	SPACE_PRESSED = false;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Function to render text
void renderText(unsigned int shaderProgram, std::map<char, Character>& characters, std::string text, float x, float y, float z, float scale, glm::mat4 view, glm::mat4 projection) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Number of layers for 3D effect
    int numLayers = 5;
    float depthStep = 0.05f; // Small z-offset per layer
    float x_offset = x;

    for (char c : text) {
        Character ch = characters[c];

        float xpos = x_offset + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos,     ypos,     0.0f, 1.0f },
            { xpos + w, ypos,     1.0f, 1.0f },
            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos,     1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };

        // Render multiple layers
        for (int i = 0; i < numLayers; ++i) {
            float depth = z - i * depthStep;
            // Gradually darken the color for deeper layers
            float colorFactor = 1.0f - (i * 0.15f); // Darken by 15% per layer
            glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), colorFactor, colorFactor, colorFactor);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, depth));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        x_offset += (ch.advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDisable(GL_BLEND);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL 3D Text", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        glfwTerminate();
        return -1;
    }

    FT_Face face;
    // Try DejaVuSans.ttf (Ubuntu default) or arial.ttf if installed
    // Use /usr/share/fonts/truetype/msttcorefonts/arial.ttf if ttf-mscorefonts-installer is installed
    // Or copy a .ttf file to the project directory and use ./filename.ttf
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        glfwTerminate();
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::map<char, Character> characters;
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        characters[c] = character;
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Shaders
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
        out vec2 TexCoords;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;
        uniform sampler2D text;
        uniform vec3 textColor;
        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor, 1.0) * sampled;
        }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnable(GL_DEPTH_TEST);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f);

        glUseProgram(shaderProgram);
        glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), 1.0f, 1.0f, 1.0f);

        
        // Render "Hello World" at position (-2, 0, 0) with scale 0.02
        renderText(shaderProgram, characters, text, -2.2f, 0.0f, 0.0f, 0.02f, view, projection);
		renderText(shaderProgram, characters, "10", -2.0f, 2.0f, 0.0f, 0.02f, view, projection);
	
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    for (auto& pair : characters) {
        glDeleteTextures(1, &pair.second.textureID);
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
