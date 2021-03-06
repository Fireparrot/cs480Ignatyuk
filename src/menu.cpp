#include "menu.h"
#include "shader.h"
#include <utility>

#include <glm/ext.hpp>

MenuItem::MenuItem(Menu * menu_,
        glm::vec3 position_, glm::vec2 size_,
        GLint overlayTexture_, GLint texture_, GLint hoverTexture_, GLint clickTexture_,
        std::function<void (Menu * menu, MenuItem * item, float x, float y)> function_,
        bool actOnClick_, float slider_, float sliderMinX_, float sliderMaxX_):
    menu(menu_),
    position(position_),
    size(size_),
    overlayTexture(overlayTexture_),
    texture(texture_),
    hoverTexture(hoverTexture_),
    clickTexture(clickTexture_),
    function(function_),
    actOnClick(actOnClick_),
    slider(slider_),
    sliderMinX(sliderMinX_),
    sliderMaxX(sliderMaxX_)
{}
MenuItem::~MenuItem() {}

void setSlider(MenuItem * item, float x, float y) {
    x *= 800; y *= 500;
    float minX, maxX;
    minX = item->position.x+item->size.x/2*item->sliderMinX;
    maxX = item->position.x+item->size.x/2*item->sliderMaxX;
    item->slider = x < minX ? 0 : x > maxX ? 1 : (x-minX)/(maxX-minX);
}

Menu::Menu(Graphics * graphics_, std::string vertShaderFile, std::string fragShaderFile):
    graphics(graphics_),
    hasHover(false),
    hasClick(false),
    activeID(0),
    exitRequest(false)
{
    shader = new Shader();
    if(!shader->Initialize()) {
        printf("Shader (menu) failed to Initialize\n");
        return;
    }
    if(!shader->AddShader(vertShaderFile, GL_VERTEX_SHADER)) {
        printf("Vertex Shader (menu) failed to Initialize\n");
        return;
    }
    if(!shader->AddShader(fragShaderFile, GL_FRAGMENT_SHADER)) {
        printf("Fragment Shader (menu) failed to Initialize\n");
        return;
    }
    if(!shader->Finalize()) {
        printf("Program (menu) failed to Finalize\n");
        return;
    }
    
    vertices = {{{-0.5f, -0.5f, 0}, {0, 0}, {0, 0, -1}},
                {{ 0.5f, -0.5f, 0}, {1, 0}, {0, 0, -1}},
                {{-0.5f,  0.5f, 0}, {0, 1}, {0, 0, -1}},
                {{ 0.5f,  0.5f, 0}, {1, 1}, {0, 0, -1}},};
    indices = {0, 1, 2, 3, 2, 1};
    
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
    
    projMat = glm::ortho(-800.f, 800.f, -500.f, 500.f, 0.01f, 100.0f);
}
Menu::~Menu() {
    delete shader;
    for(MenuItem * item : items) {delete item;}
}

void Menu::add(glm::vec3 pos, glm::vec2 size,
        GLint overlayTexture, GLint texture, GLint hoverTexture, GLint clickTexture,
        std::function<void (Menu * menu, MenuItem * item, float x, float y)> function,
        bool actOnClick, float slider, float sliderMinX, float sliderMaxX) {
    items.push_back(new MenuItem(this,
            pos, size,
            overlayTexture, texture, hoverTexture, clickTexture,
            function,
            actOnClick, slider, sliderMinX, sliderMaxX));
}
void Menu::add(glm::vec2 lb, glm::vec2 rt, float depth,
        GLint overlayTexture, GLint texture, GLint hoverTexture, GLint clickTexture,
        std::function<void (Menu * menu, MenuItem * item, float x, float y)> function,
        bool actOnClick, float slider, float sliderMinX, float sliderMaxX) {
    items.push_back(new MenuItem(this,
            glm::vec3((lb.x+rt.x)/2, (lb.y+rt.y)/2, depth), glm::vec2(rt.x-lb.x, rt.y-lb.y),
            overlayTexture, texture, hoverTexture, clickTexture,
            function,
            actOnClick, slider, sliderMinX, sliderMaxX));
}
void Menu::addGroup(std::vector<uint> group) {
    actives.push_back(std::move(group));
}

bool Menu::on(float x, float y, uint & itemID) const {
    x *= 800;
    y *= 500;
    bool found = false;
    for(uint i : actives[activeID]) {
        glm::vec3 & pos = items[i]->position;
        glm::vec2 & size = items[i]->size;
        if(x <= pos.x+size.x/2 && x >= pos.x-size.x/2 && y <= pos.y+size.y/2 && y >= pos.y-size.y/2) {
            if(found) {
                if(pos.z < items[itemID]->position.z) {itemID = i;}
            } else {
                itemID = i;
                found = true;
            }
        }
    }
    return found;
}
void Menu::mouseHover(float x, float y) {
    if(!hasClick) {
        hasHover = on(x, y, hover);
    } else {
        if(!items[click]->actOnClick) {
            items[click]->function(this, items[click], x, y);
        }
    }
}
void Menu::mouseClick(float x, float y) {
    hasClick = on(x, y, click);
}
void Menu::mouseRelease(float x, float y) {
    uint itemID;
    bool isOnSomeItem = on(x, y, itemID);
    if(isOnSomeItem && itemID == click && items[click]->actOnClick) {
        items[click]->function(this, items[click], x, y);
    }
    hasClick = false;
}
void Menu::render() const {
    shader->Enable();
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tex));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    
    for(uint i : actives[activeID]) {
        MenuItem * item = items[i];
        
        GLint texture = item->texture;
        if(hasHover && i == hover && item->hoverTexture != -1) {texture = item->hoverTexture;}
        if(hasClick && i == click && item->clickTexture != -1) {texture = item->clickTexture;}
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glUniformMatrix4fv(shader->uniform("projMat"), 1, GL_FALSE, glm::value_ptr(projMat)); 
        glUniformMatrix4fv(shader->uniform("viewMat"), 1, GL_FALSE, glm::value_ptr(glm::lookAt(glm::vec3{0, 0, 1}, glm::vec3{0, 0, -1}, glm::vec3{0, 1, 0})));
        glUniformMatrix4fv(shader->uniform("modelMat"), 1, GL_FALSE, glm::value_ptr(glm::translate(item->position+glm::vec3{0, 0, -1})*glm::scale(glm::vec3{item->size.x, item->size.y, 1.0f})));
        glUniform1i(shader->uniform("tex"), 0);
    
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        
        texture = item->overlayTexture == -1 ? 0 : item->overlayTexture;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        float minX, maxX;
        minX = item->size.x/2*item->sliderMinX;
        maxX = item->size.x/2*item->sliderMaxX;
        glUniformMatrix4fv(shader->uniform("projMat"), 1, GL_FALSE, glm::value_ptr(projMat)); 
        glUniformMatrix4fv(shader->uniform("viewMat"), 1, GL_FALSE, glm::value_ptr(glm::lookAt(glm::vec3{0, 0, 1}, glm::vec3{0, 0, -1}, glm::vec3{0, 1, 0})));
        glUniformMatrix4fv(shader->uniform("modelMat"), 1, GL_FALSE, glm::value_ptr(
                glm::translate(item->position+glm::vec3(minX+item->slider*(maxX-minX), 0, 0))*
                glm::scale(glm::vec3{item->size.x, item->size.y, 1.0f})));
        glUniform1i(shader->uniform("tex"), 0);
    
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    
    }
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
