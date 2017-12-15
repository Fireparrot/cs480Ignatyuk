#ifndef MENU_H
#define MENU_H

#include <vector>
#include <functional>
#include "graphics_headers.h"

using uint = unsigned int;

class Graphics;
class Shader;
class Menu;
class MenuItem {
    friend class Menu;
    friend class Graphics;
private:
    Menu * menu;
    glm::vec3 position;
    glm::vec2 size;
    GLint overlayTexture;
    GLint texture;
    GLint hoverTexture;
    GLint clickTexture;
    std::function<void (Menu *, MenuItem *)> function;
public:
    MenuItem(Menu * menu_,
            glm::vec3 position_, glm::vec2 size_,
            GLint overlayTexture_, GLint texture_, GLint hoverTexture_ = -1, GLint clickTexture_ = -1,
            std::function<void (Menu *, MenuItem *)> function_ = [] (Menu * menu, MenuItem * item) {});
    ~MenuItem();
};

class Menu {
    friend class MenuItem;
    friend class Graphics;
private:
    std::vector<MenuItem *> items;
    std::vector<std::vector<uint>> actives;
    bool hasHover, hasClick;
    uint hover, click;
    glm::mat4 projMat;
    std::vector<VertexData> vertices;
    std::vector<uint> indices;
    GLuint VB;
    GLuint IB;
    Shader * shader;
public:
    const Graphics * graphics;
    uint activeID;
    bool exitRequest;
public:
    Menu(Graphics * graphics_, std::string vertShaderFile, std::string fragShaderFile);
    ~Menu();
private:
    bool on(float x, float y, uint & itemID) const;
public:
    void add(glm::vec3 pos, glm::vec2 size,
            GLint overlayTexture, GLint texture, GLint hoverTexture = -1, GLint clickTexture = -1,
            std::function<void (Menu *, MenuItem *)> function = [] (Menu * menu, MenuItem * item) {});
    void add(glm::vec2 bl, glm::vec2 tr, float depth,
            GLint overlayTexture, GLint texture, GLint hoverTexture = -1, GLint clickTexture = -1,
            std::function<void (Menu *, MenuItem *)> function = [] (Menu * menu, MenuItem * item) {});
    void addGroup(std::vector<uint> group);
    
    void mouseHover(float x, float y);
    void mouseClick(float x, float y);
    void mouseRelease(float x, float y);
    void render() const;
};

#endif

