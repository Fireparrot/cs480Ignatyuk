#ifndef IMAGE_HELPER_H
#define IMAGE_HELPER_H

#include <string>
#include <map>
#include "graphics_headers.h"
#include <IL/il.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>

#undef __FTERRORS_H__
    #define FT_ERRORDEF( e, v, s )  { e, s },
    #define FT_ERROR_START_LIST     {
    #define FT_ERROR_END_LIST       { 0, NULL } };
    const struct {
        int          err_code;
        const char*  err_msg;
    } ft_errors[] =
    #include FT_ERRORS_H

using uint = unsigned int;
using ulong = unsigned long;

class ImageHelper {
private:
    const std::string texturePath = "objects/textures/";
    const static uint MAX_GLYPHS = 128;
    const std::string fontPath = "/usr/share/fonts/";
    FT_Library ftLibrary;
    mutable std::map<std::string, FT_Face> fonts;
    std::string fontName;
    mutable FT_Glyph glyphs[MAX_GLYPHS];
    ulong fontSize;
    float space;
    float fr, fg, fb, fa;
    float br, bg, bb, ba;
    uint leftPadding, rightPadding, bottomPadding, topPadding;
    uint minWidth, minHeight;
    int horAdjust, verAdjust;
    bool usePadding;
public:
    ImageHelper();
    ~ImageHelper();
public:
    GLuint loadTexture(const std::string filename, ILuint * imagePtr = NULL) const;
    void setForeColor(float r, float g, float b, float a);
    void setBackColor(float r, float g, float b, float a);
    bool setFont(const std::string fontName_);
    bool setFontSize(ulong size);
    void setSpace(float space_);
    void setMin(uint minWidth_, uint minHeight_);
    void setPadding(uint left, uint right, uint bottom, uint top);
    void setHorAdjust(int adj);
    void setVerAdjust(int adj);
    void setUsePadding(bool b);
    GLuint rasterizeText(const std::string text, ILuint * imagePtr = NULL) const;
    GLuint rasterizeText(const std::wstring text, ILuint * imagePtr = NULL) const;
    void loadTexture(GLuint tex, ILuint * imagePtr = NULL) const;
    void paint(glm::vec4 color, glm::vec2 pos, float radius, ILuint * imagePtr, bool scaleHorizontally = true);
    void paint(glm::vec4 color, glm::vec2 start, glm::vec2 end, float radius, ILuint * imagePtr, bool scaleHorizontally = true);
};

#endif

