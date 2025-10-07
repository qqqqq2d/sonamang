#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <sstream>
#include <set>
#include <map>
#include <math.h>
#include <cstdint>

#define NUM_POINTS 500

std::string sõna;
std::ifstream sonade_list("sonad3.txt");

std::string kombinatsioon;
std::ifstream kombinatsioonide_list("kombinatsioonid3.txt");

std::vector<std::string> kombinatsioonid{};
std::vector<std::string> sõnad{};

int suvaline_kombinatsioon;
std::random_device dev;
std::mt19937 rng(dev());

std::string sõnavahetus() {
    while (std::getline(kombinatsioonide_list, kombinatsioon)) {
        kombinatsioonid.push_back(kombinatsioon);
    }

    std::uniform_int_distribution<std::mt19937::result_type> kombinatsiooni_vahemik(0, kombinatsioonid.size()-1);
    suvaline_kombinatsioon = kombinatsiooni_vahemik(rng);
    return kombinatsioonid[suvaline_kombinatsioon];
}

void vilkumine(float &opacity, std::string choice, float multiplier, float a_m) {
    if (choice == "up")
        opacity += 0.01f * multiplier * a_m;
    else {
        opacity -= 0.01f * multiplier * a_m;
    }
}

void näita_sõnu(std::vector<std::string> sõnad) {
    for (auto i : sõnad) {
        std::cout << i << " ";
    }
    std::cout << sõnad.size() << std::endl;
}

struct KeyMapping {
    int glfw_key;
    std::string character;
};

KeyMapping key_map[] = {
    {GLFW_KEY_A, "A"}, {GLFW_KEY_B, "B"}, {GLFW_KEY_C, "C"}, {GLFW_KEY_D, "D"},
    {GLFW_KEY_E, "E"}, {GLFW_KEY_F, "F"}, {GLFW_KEY_G, "G"}, {GLFW_KEY_H, "H"},
    {GLFW_KEY_I, "I"}, {GLFW_KEY_J, "J"}, {GLFW_KEY_K, "K"}, {GLFW_KEY_L, "L"},
    {GLFW_KEY_M, "M"}, {GLFW_KEY_N, "N"}, {GLFW_KEY_O, "O"}, {GLFW_KEY_P, "P"},
    {GLFW_KEY_Q, "Q"}, {GLFW_KEY_R, "R"}, {GLFW_KEY_S, "S"}, {GLFW_KEY_T, "T"},
    {GLFW_KEY_U, "U"}, {GLFW_KEY_V, "V"}, {GLFW_KEY_W, "W"}, {GLFW_KEY_X, "X"},
    {GLFW_KEY_Y, "Y"}, {GLFW_KEY_Z, "Z"}, {GLFW_KEY_1, "Š"}, {GLFW_KEY_2, "Ž"},
    {GLFW_KEY_SPACE, "Õ"}, {GLFW_KEY_4, "Ä"}, {GLFW_KEY_5, "Ö"}, {GLFW_KEY_6, "Ü"}
};

std::vector<FT_UInt> decodeUTF8(const std::string& text) {
    std::vector<FT_UInt> codePoints;
    size_t i = 0;
    while (i < text.size()) {
        FT_UInt codePoint = 0;
        unsigned char c = text[i];
        if (c < 0x80) {
            codePoint = c;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            codePoint = ((c & 0x1F) << 6) | (text[i + 1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            codePoint = ((c & 0x0F) << 12) | ((text[i + 1] & 0x3F) << 6) | (text[i + 2] & 0x3F);
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            codePoint = ((c & 0x07) << 18) | ((text[i + 1] & 0x3F) << 12) | ((text[i + 2] & 0x3F) << 6) | (text[i + 3] & 0x3F);
            i += 4;
        } else {
            i += 1;
            continue;
        }
        codePoints.push_back(codePoint);
    }
    return codePoints;
}

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

Camera camera;
bool firstMouse = true;
float lastX = 400, lastY = 300;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float fps = 0.0f;
float fpsUpdateInterval = 0.5f;
float fpsAccumulatedTime = 0.0f;
int frameCount = 0;
int totalFrames = 0;

struct Character {
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

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


bool lastCharIsMultibyte(const std::string& s) {
    if (s.empty()) return false;

    // Walk backwards to find the start of the last UTF-8 character
    int i = s.size() - 1;
    // Move left while the byte is a UTF-8 continuation byte (10xxxxxx)
    while (i > 0 && (static_cast<unsigned char>(s[i]) & 0xC0) == 0x80)
        --i;

    // Length of last character in bytes
    size_t charLen = s.size() - i;

    // If more than one byte, it's a multibyte character (non-ASCII)
    return charLen > 1;
}

std::vector<bool> täht_vajutatud(32, false);
std::string sisendi_tekst = "";
bool SPACE_PRESSED = false;
bool ENTER_PRESSED = false;
bool BACKSPACE_PRESSED = false;
bool text_entered = false;
std::string kombinatsiooni_tekst = sõnavahetus();

void processInput(GLFWwindow* window, bool mäng_läbi) {
    if (!mäng_läbi) {
        float cameraSpeed = 2.5f * deltaTime;
        
        for (int i = 0; i < 26; i++) {
            int key = key_map[i].glfw_key;
            if (glfwGetKey(window, key) == GLFW_PRESS && !täht_vajutatud[i]) {
                täht_vajutatud[i] = true;
                sisendi_tekst += key_map[i].character;
            }
            if (glfwGetKey(window, key) == GLFW_RELEASE) {
                täht_vajutatud[i] = false;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS && !täht_vajutatud[26]) {
            täht_vajutatud[26] = true;
            sisendi_tekst += "Ö";
        }
        if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_RELEASE) {
            täht_vajutatud[26] = false;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS && !täht_vajutatud[27]) {
            täht_vajutatud[27] = true;
            sisendi_tekst += "Ü";
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_RELEASE) {
            täht_vajutatud[27] = false;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS && !täht_vajutatud[28]) {
            täht_vajutatud[28] = true;
            sisendi_tekst += "Õ";
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_RELEASE) {
            täht_vajutatud[28] = false;
        }
        if (glfwGetKey(window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS && !täht_vajutatud[29]) {
            täht_vajutatud[29] = true;
            sisendi_tekst += "Ä";
        }
        if (glfwGetKey(window, GLFW_KEY_APOSTROPHE) == GLFW_RELEASE) {
            täht_vajutatud[29] = false;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !täht_vajutatud[30]) {
            täht_vajutatud[30] = true;
            sisendi_tekst += "Š";
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
            täht_vajutatud[30] = false;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !täht_vajutatud[31]) {
            täht_vajutatud[31] = true;
            sisendi_tekst += "Ž";
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
            täht_vajutatud[31] = false;
        }
        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS && !BACKSPACE_PRESSED) {
        	int n = 1;
        	
            if (!sisendi_tekst.empty()) {
            	if (lastCharIsMultibyte(sisendi_tekst))
            		n = 2;
            	sisendi_tekst.erase(sisendi_tekst.length() - n);
            }
            BACKSPACE_PRESSED = true;
        }
        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_RELEASE) {
            BACKSPACE_PRESSED = false;
        }
    }
    
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !ENTER_PRESSED) {
        ENTER_PRESSED = true;
        text_entered = true;
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
        ENTER_PRESSED = false;
        text_entered = false;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void renderText(unsigned int shaderProgram, std::map<FT_UInt, Character>& characters, std::string text, float x, float y, float z, float scale, glm::mat4 view, glm::mat4 projection, bool is2D, float opacity, glm::vec3 gradientStartColor, glm::vec3 gradientEndColor, bool mäng_läbi = false) {
    if (!mäng_läbi) {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glm::mat4 proj = is2D ? glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f) : projection;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniform1f(glGetUniformLocation(shaderProgram, "opacity"), opacity);
        glUniform3fv(glGetUniformLocation(shaderProgram, "gradientStartColor"), 1, glm::value_ptr(gradientStartColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "gradientEndColor"), 1, glm::value_ptr(gradientEndColor));
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (is2D) {
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
        }
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
        std::vector<FT_UInt> codePoints = decodeUTF8(text);
        float x_offset = x;
        for (FT_UInt c : codePoints) {
            auto it = characters.find(c);
            if (it == characters.end()) {
                std::cerr << "WARNING: Glyph not found for code point " << c << std::endl;
                continue;
            }
            Character ch = it->second;
            float xpos = x_offset + ch.bearing.x * scale;
            float ypos = is2D ? (600.0f - y - (ch.size.y - ch.bearing.y) * scale) : (y - (ch.size.y - ch.bearing.y) * scale);
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
            if (!is2D) {
                int numLayers = 5;
                float depthStep = 0.05f;
                for (int i = 0; i < numLayers; ++i) {
                    float depth = z - i * depthStep;
                    float colorFactor = 1.0f - (i * 0.15f);
                    glUniform3f(glGetUniformLocation(shaderProgram, "gradientStartColor"), gradientStartColor.x * colorFactor, gradientStartColor.y * colorFactor, gradientStartColor.z * colorFactor);
                    glUniform3f(glGetUniformLocation(shaderProgram, "gradientEndColor"), gradientEndColor.x * colorFactor, gradientEndColor.y * colorFactor, gradientEndColor.z * colorFactor);
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
            } else {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
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
}

void renderSquare(unsigned int shaderProgram, unsigned int whiteTexture, float x, float y, float size, glm::vec3 color, float opacity) {
    glUseProgram(shaderProgram);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "gradientStartColor"), 1, glm::value_ptr(color));
    glUniform3fv(glGetUniformLocation(shaderProgram, "gradientEndColor"), 1, glm::value_ptr(color));
    glUniform1f(glGetUniformLocation(shaderProgram, "opacity"), opacity);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
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
    float halfSize = size / 2.0f;
    float vertices[6][4] = {
        { x - halfSize, y - halfSize, 0.0f, 0.0f },
        { x + halfSize, y - halfSize, 1.0f, 0.0f },
        { x + halfSize, y + halfSize, 1.0f, 1.0f },
        { x - halfSize, y - halfSize, 0.0f, 0.0f },
        { x + halfSize, y + halfSize, 1.0f, 1.0f },
        { x - halfSize, y + halfSize, 0.0f, 1.0f }
    };
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDisable(GL_BLEND);
}

// Function to generate heart shape vertices
void generateHeartVertices(float* vertices, int numPoints) {
    for (int i = 0; i < numPoints; i++) {
        float t = 2.0f * M_PI * i / numPoints;
        float x = 16.0f * pow(sin(t), 3);
        float y = 13.0f * cos(t) - 5.0f * cos(2.0f * t) - 2.0f * cos(3.0f * t) - cos(4.0f * t);
        vertices[i * 4] = x;
        vertices[i * 4 + 1] = y;
        vertices[i * 4 + 2] = 0.0f; // Texture coordinate u
        vertices[i * 4 + 3] = 0.0f; // Texture coordinate v
    }
}

void renderHeart(unsigned int shaderProgram, unsigned int whiteTexture, float x, float y, float scale, glm::vec3 color, float opacity) {
    glUseProgram(shaderProgram);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "gradientStartColor"), 1, glm::value_ptr(color));
    glUniform3fv(glGetUniformLocation(shaderProgram, "gradientEndColor"), 1, glm::value_ptr(color));
    glUniform1f(glGetUniformLocation(shaderProgram, "opacity"), opacity);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    float vertices[NUM_POINTS * 4];
    generateHeartVertices(vertices, NUM_POINTS);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_POINTS);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDisable(GL_BLEND);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Sõna mäng", nullptr, nullptr);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, false);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //glfwSwapInterval( 0 );
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        glfwTerminate();
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, "./Gentium-R.ttf", 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        glfwTerminate();
        return -1;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    std::map<FT_UInt, Character> characters;
    for (FT_UInt c = 0; c <= 0xFF; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph for code point " << c << std::endl;
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
    unsigned int whiteTexture;
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    unsigned char whitePixel = 255;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_UNSIGNED_BYTE, &whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex;
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
        uniform vec3 gradientStartColor;
        uniform vec3 gradientEndColor;
        uniform float opacity;
        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            vec3 finalColor = mix(gradientEndColor, gradientStartColor, TexCoords.y);
            color = vec4(finalColor, opacity) * sampled;
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
    int elud = 3;
    bool vastus = true;
    std::vector<std::string> kombinatsiooni_sõnad;
    std::string vaadeldav_sõna;
    while (std::getline(sonade_list, sõna)) {
        sõnad.push_back(sõna);
    }
    float whole_scene_opacity = 0.0f;
    bool sõnad_push_back_done = false;
    
    // Vilkumise muutujad
    bool opacity_up = true;
    bool vale_vastus_vilkumine = false;
    
    auto terve_vilkumine = []() -> void {
        std::cout << "hello" << std::endl;
    };
    
    int skoor = 0;
    std::string skoor_tekst = "Skoor: " + std::to_string(skoor);
    std::string skoor_lõpp_tekst = "Sinu skoor: " + std::to_string(skoor);
    
    float aeg = 0;
    float vastupidine_aeg = 0;
    int aeg_int;
    int sekundite_arv = 10;
    
    
    bool mäng_läbi = false;
    while (!glfwWindowShouldClose(window)) {
        float current_time = glfwGetTime();
        float a_m = deltaTime * 62.5; 
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fpsAccumulatedTime += deltaTime;
        frameCount++;
        
        if (fpsAccumulatedTime >= fpsUpdateInterval) {
            fps = frameCount / fpsAccumulatedTime;
            frameCount = 0;
            fpsAccumulatedTime = 0.0f;
        }

    	aeg = aeg + 0.0166 * a_m;
    	vastupidine_aeg = (sekundite_arv+1) - aeg;
    	aeg_int = static_cast<int>(vastupidine_aeg);
    	std::string aeg_tekst = std::to_string(aeg_int);
    	
    	if (aeg >= sekundite_arv && !mäng_läbi) {
    		//std::cout << aeg/current_time << std::endl;
    		//exit(1);
    		aeg = 0;
    		elud--;
    		kombinatsiooni_tekst = sõnavahetus();
            kombinatsiooni_sõnad.clear();
            sõnad_push_back_done = false;
            vale_vastus_vilkumine = true;
            sisendi_tekst = "";
    	}
    
        if (!sõnad_push_back_done) {
            for (auto i : sõnad) {
                size_t pos = i.find(kombinatsiooni_tekst);
                if (pos != std::string::npos) {
                    kombinatsiooni_sõnad.push_back(i);
                }
            }
            sõnad_push_back_done = true;
            näita_sõnu(kombinatsiooni_sõnad);
        }
        if (text_entered) {
            vastus = false;
            for (auto i : kombinatsiooni_sõnad) {
                if (i == sisendi_tekst) {
                    sõnad.erase(std::remove(sõnad.begin(), sõnad.end(), sisendi_tekst), sõnad.end());
                    kombinatsiooni_tekst = sõnavahetus();
                    kombinatsiooni_sõnad.clear();
                    sõnad_push_back_done = false;
                    vastus = true;
                    skoor++;
                    skoor_tekst = "Skoor: " + std::to_string(skoor);
                    skoor_lõpp_tekst = "Sinu skoor: " + std::to_string(skoor);
                    aeg = 0;
                    break;
                }
            }
            if (sisendi_tekst != "" && !vastus) {
                elud--;
                kombinatsiooni_tekst = sõnavahetus();
                kombinatsiooni_sõnad.clear();
                sõnad_push_back_done = false;
                vale_vastus_vilkumine = true;
                aeg = 0;
            }
            sisendi_tekst = "";
        }
        
        if (elud == 0) {
            vilkumine(whole_scene_opacity, "up", 2, a_m);
            mäng_läbi = true;
        }
        
        if (vale_vastus_vilkumine) {
            if (opacity_up)
                vilkumine(whole_scene_opacity, "up", 10, a_m);
            if (whole_scene_opacity >= 0.75f) {
                opacity_up = false;
            }
            if (!opacity_up) {
                vilkumine(whole_scene_opacity, "down", 1.4, a_m);
            }
            if (whole_scene_opacity <= 0) {
                whole_scene_opacity = 0.0f;
                vale_vastus_vilkumine = false;
                opacity_up = true;
            }
        }
        
        processInput(window, mäng_läbi);
        glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glUseProgram(shaderProgram);
        renderText(shaderProgram, characters, kombinatsiooni_tekst, -3.0f, 0.0f, 0.0f, 0.02f, view, projection, false, 1.0f, glm::vec3(1.0f, 1.0f, 01.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        renderText(shaderProgram, characters, aeg_tekst, -2.4f, 1.0f, 0.0f, 0.01f, view, projection, false, 1.0f, glm::vec3(1.0f, 1.0f, 01.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // Render hearts instead of elutext
        for (int i = 0; i < elud; ++i) {
            renderHeart(shaderProgram, whiteTexture, 50.0f + i * 50.0f, 565.0f, 0.9f, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f - whole_scene_opacity);
        }
        std::stringstream fpsText;
        fpsText << "FPS: " << static_cast<float>(fps);
        renderText(shaderProgram, characters, sisendi_tekst, 80.0f, 500.0f, 100.0f, 1.5f, glm::mat4(1.0f), projection, true, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.5f));
        renderText(shaderProgram, characters, skoor_tekst, 600.0f, 48.0f, 100.0f, 0.75f, glm::mat4(1.0f), projection, true, 1.0f, glm::vec3(1.0f, 1.5f, 1.0f), glm::vec3(1.02f, 1.0f, 0.9f));
        renderSquare(shaderProgram, whiteTexture, 400.0f, 300.0f, 5.0f, glm::vec3(1.0f, 0.0f, 0.0f), 0.5f);
        renderSquare(shaderProgram, whiteTexture, 400.0f, 300.0f, 1000.0f, glm::vec3(0.5f, 0.0f, 0.0f), whole_scene_opacity);
        renderText(shaderProgram, characters, "MÄNG LÄBI", 200.0f, 300.0f, 100.0f, 1.5f, glm::mat4(1.0f), projection, true, 1.0f, glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), !mäng_läbi);
        renderText(shaderProgram, characters, skoor_lõpp_tekst, 270.0f, 380.0f, 100.0f, 0.75f, glm::mat4(1.0f), projection, true, 1.0f, glm::vec3(1.0f, 1.5f, 1.0f), glm::vec3(1.02f, 1.0f, 0.9f), !mäng_läbi);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    for (auto& pair : characters) {
        glDeleteTextures(1, &pair.second.textureID);
    }
    glDeleteTextures(1, &whiteTexture);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
