#include "image_helper.h"
#include <iostream>
#include <algorithm>

ImageHelper::ImageHelper():
    fr(0), fg(0), fb(0), fa(1),
    br(0), bg(0), bb(0), ba(0),
    fontSize(16<<6),
    space(0.2f),
    leftPadding(0), rightPadding(0), bottomPadding(0), topPadding(0),
    minWidth(0), minHeight(0),
    horAdjust(0), verAdjust(0),
    usePadding(true)
{
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    
    auto error = FT_Init_FreeType(&ftLibrary);
    if(error) {
        std::cout << "Error within FreeType! " << error << std::endl;
    }
}
ImageHelper::~ImageHelper() {
    
}
    
//This is a helper function that uses devIL to open an image, then loads it into openGL and passes back the openGL reference to the texture
GLuint ImageHelper::loadTexture(const std::string filename, ILuint * imagePtr) const {
    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);
    ilLoadImage((texturePath + filename).c_str());
    
    GLuint tex;
    glGenTextures(1, &tex);
    loadTexture(tex);
    
    
    if(imagePtr) {
        *imagePtr = image;
    } else {
        ilDeleteImages(1, &image);
    }
    
    return tex;
}
void ImageHelper::setForeColor(float r, float g, float b, float a) {
    fr = r;
    fg = g;
    fb = b;
    fa = a;
}
void ImageHelper::setBackColor(float r, float g, float b, float a) {
    br = r;
    bg = g;
    bb = b;
    ba = a;
}

bool ImageHelper::setFont(const std::string fontName_) {
    auto it = fonts.find(fontName_);
    if(it != fonts.end()) {
        for(int i = 0; i < MAX_GLYPHS; ++i) {FT_Done_Glyph(glyphs[i]);}
        
        fontName = fontName_;
        
        auto error = FT_Set_Pixel_Sizes(it->second, 0, fontSize);
        if(error) {
            std::cout << "Couldn't set proper font size! " << ft_errors[error].err_msg << std::endl;
        }
        
        for(int i = 0; i < MAX_GLYPHS; ++i) {
            FT_Load_Glyph(it->second, FT_Get_Char_Index(it->second, i), FT_LOAD_DEFAULT);
            FT_Get_Glyph(it->second->glyph, &glyphs[i]);
            error = FT_Glyph_To_Bitmap(&glyphs[i], FT_RENDER_MODE_NORMAL, 0, 1 );
            if (error) {
                std::cout << "Couldn't convert glyph to bitmap! " << ft_errors[error].err_msg << std::endl;
            }
        }
        return true;
    }
    
    FT_Face temp;
    auto error = FT_New_Face(ftLibrary, /*(fontPath+fontName_).c_str()*/"/usr/share/fonts/truetype/fonts-japanese-gothic.ttf", 0, &temp);
    if(error) {
        std::cout << "Couldn't load font (" << fontPath << ")" << fontName_ << "! " << ft_errors[error].err_msg << std::endl;
        return false;
    } else {
        error = FT_Set_Pixel_Sizes(temp, 0, fontSize);
        if(error) {
            std::cout << "Couldn't set proper font size! " << ft_errors[error].err_msg << std::endl;
        }
        fontName = fontName_;
        fonts[fontName] = temp;
        
        for(int i = 0; i < MAX_GLYPHS; ++i) {
            FT_Load_Glyph(fonts[fontName], FT_Get_Char_Index(fonts[fontName], i), FT_LOAD_DEFAULT);
            FT_Get_Glyph(fonts[fontName]->glyph, &glyphs[i]);
            error = FT_Glyph_To_Bitmap(&glyphs[i], FT_RENDER_MODE_NORMAL, 0, 1 );
            if(error) {
                std::cout << "Couldn't convert glyph to bitmap! " << ft_errors[error].err_msg << std::endl;
            }
        }
        
        return true;
    }
}
bool ImageHelper::setFontSize(ulong size) {
    fontSize = size;
    if(fontName != "") {
        auto error = FT_Set_Pixel_Sizes(fonts[fontName], 0, fontSize);
        if(error) {
            std::cout << "Couldn't set proper font size! " << ft_errors[error].err_msg << std::endl;
        }
    }
}
void ImageHelper::setSpace(float space_) {space = space_;}
void ImageHelper::setMin(uint minWidth_, uint minHeight_) {minWidth = minWidth_; minHeight = minHeight_;}
void ImageHelper::setPadding(uint left, uint right, uint bottom, uint top) {leftPadding = left; rightPadding = right; bottomPadding = bottom; topPadding = top;}
void ImageHelper::setHorAdjust(int adj) {horAdjust = adj < 0 ? -1 : adj > 0 ? 1 : 0;}
void ImageHelper::setVerAdjust(int adj) {verAdjust = adj < 0 ? -1 : adj > 0 ? 1 : 0;}
void ImageHelper::setUsePadding(bool b) {usePadding = b;}

GLuint ImageHelper::rasterizeText(const std::string text, ILuint * imagePtr) const {
    if(fontName == "") {
        std::cout << "No font loaded!" << std::endl;
        return 0;
    }
    
    uint width = 0;
    uint height = 0;
    uint heightNeg = 0;
    int startX = 0;
    int startY = 0;
    
    for(uint i = 0; text[i] != 0; ++i) {
        FT_BitmapGlyph glyph = (FT_BitmapGlyph)glyphs[text[i]];
        
        width += glyph->bitmap.width;
        if(text[i+1] != 0) {width += uint(fontSize*space);}
        if(text[i] == ' ') {width += uint(fontSize*2*space);} 
        height = std::max(height, 2*glyph->bitmap.rows-glyph->top);
        heightNeg = std::max(heightNeg, glyph->bitmap.rows-glyph->top);
    }
    
    if(usePadding) {
        startX = leftPadding;
        startY = bottomPadding;
        width += leftPadding+rightPadding;
        height += bottomPadding+topPadding;
    } else {
        startX = (minWidth-(int)width)/2;
        startY = (minHeight-(int)height)/2;
        width = std::max(width, minWidth);
        height = std::max(height, minHeight);
    }
    
    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);
    ilTexImage(width, height, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
    
    int offsetX = 0;
    ILubyte * imageData = ilGetData();
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            imageData[4*y*width + 4*x + 0] = ILubyte(255*br);
            imageData[4*y*width + 4*x + 1] = ILubyte(255*bg);
            imageData[4*y*width + 4*x + 2] = ILubyte(255*bb);
            imageData[4*y*width + 4*x + 3] = ILubyte(255*ba);
        }
    }
    for(int i = 0; text[i] != 0; ++i) {
        FT_BitmapGlyph glyph = (FT_BitmapGlyph)glyphs[text[i]];
        
        int offsetY = glyph->bitmap.rows-glyph->top;
        for(int y = 0; y < height; ++y) {
            for(int x = 0; x < glyph->bitmap.width; ++x) {
                if(glyph->top+heightNeg+startY > y && offsetY+y+1 > heightNeg+startY && x+offsetX+startX >= 0 && x+offsetX+startX < width) {
                    ILubyte intensity = glyph->bitmap.buffer[(glyph->top-(y-heightNeg-startY)-1)*glyph->bitmap.pitch + x];
                    imageData[4*y*width + 4*(x+offsetX+startX) + 0] *= 1-intensity/255.f;
                    imageData[4*y*width + 4*(x+offsetX+startX) + 1] *= 1-intensity/255.f;
                    imageData[4*y*width + 4*(x+offsetX+startX) + 2] *= 1-intensity/255.f;
                    imageData[4*y*width + 4*(x+offsetX+startX) + 0] += ILubyte(intensity*fr);
                    imageData[4*y*width + 4*(x+offsetX+startX) + 1] += ILubyte(intensity*fg);
                    imageData[4*y*width + 4*(x+offsetX+startX) + 2] += ILubyte(intensity*fb);
                    imageData[4*y*width + 4*(x+offsetX+startX) + 3] = std::max(imageData[4*y*width + 4*(x+offsetX+startX) + 3], ILubyte(intensity * fa));
                }
            }
        }
        offsetX += glyph->bitmap.width;
        offsetX += uint(fontSize*space);
        if(text[i] == ' ') {offsetX += uint(fontSize*2*space);}
    }
    
    GLuint tex;
    glGenTextures(1, &tex);
    loadTexture(tex);
    
    ilBindImage(0);
    if(imagePtr) {
        *imagePtr = image;
    } else {
        ilDeleteImages(1, &image);
    }
    
    return tex;
}
GLuint ImageHelper::rasterizeText(const std::wstring text, ILuint * imagePtr) const {
    if(fontName == "") {
        std::cout << "No font loaded!" << std::endl;
        return 0;
    }
    
    uint width = 0;
    uint height = 0;
    uint heightNeg = 0;
    int startX = 0;
    int startY = 0;
    
    for(uint i = 0; text[i] != 0; ++i) {
        FT_BitmapGlyph glyph;
        if(text[i] < MAX_GLYPHS) {
            glyph = (FT_BitmapGlyph)glyphs[text[i]];
        } else {
            FT_Glyph temp;
            FT_Load_Glyph(fonts[fontName], FT_Get_Char_Index(fonts[fontName], (FT_ULong)text[i]), FT_LOAD_DEFAULT);
            FT_Get_Glyph(fonts[fontName]->glyph, &temp);
            auto error = FT_Glyph_To_Bitmap(&temp, FT_RENDER_MODE_NORMAL, 0, 1);
            if(error) {
                std::cout << "Couldn't convert glyph to bitmap! " << ft_errors[error].err_msg << std::endl;
            }
            glyph = (FT_BitmapGlyph)temp;
        }
        
        width += glyph->bitmap.width;
        if(text[i+1] != 0) {width += uint(fontSize*space);}
        if(text[i] == ' ') {width += uint(fontSize*2*space);} 
        height = std::max(height, 2*glyph->bitmap.rows-glyph->top);
        heightNeg = std::max(heightNeg, glyph->bitmap.rows-glyph->top);
    }
    
    if(usePadding) {
        startX = leftPadding;
        startY = bottomPadding;
        width += leftPadding+rightPadding;
        height += bottomPadding+topPadding;
    } else {
        startX = (minWidth-(int)width)/2;
        startY = (minHeight-(int)height)/2;
        width = std::max(width, minWidth);
        height = std::max(height, minHeight);
    }
    
    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);
    ilTexImage(width, height, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
    
    int offsetX = 0;
    ILubyte * imageData = ilGetData();
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            imageData[4*y*width + 4*x + 0] = ILubyte(255*br);
            imageData[4*y*width + 4*x + 1] = ILubyte(255*bg);
            imageData[4*y*width + 4*x + 2] = ILubyte(255*bb);
            imageData[4*y*width + 4*x + 3] = ILubyte(255*ba);
        }
    }
    for(int i = 0; text[i] != 0; ++i) {
        FT_BitmapGlyph glyph;
        if(text[i] < MAX_GLYPHS) {
            glyph = (FT_BitmapGlyph)glyphs[text[i]];
        } else {
            FT_Glyph temp;
            FT_Load_Glyph(fonts[fontName], FT_Get_Char_Index(fonts[fontName], (FT_ULong)text[i]), FT_LOAD_DEFAULT);
            FT_Get_Glyph(fonts[fontName]->glyph, &temp);
            auto error = FT_Glyph_To_Bitmap(&temp, FT_RENDER_MODE_NORMAL, 0, 1 );
            if(error) {
                std::cout << "Couldn't convert glyph to bitmap! " << ft_errors[error].err_msg << std::endl;
            }
            glyph = (FT_BitmapGlyph)temp;
        }
        int offsetY = glyph->bitmap.rows-glyph->top;
        for(int y = 0; y < height; ++y) {
            for(int x = 0; x < glyph->bitmap.width; ++x) {
                if(glyph->top+heightNeg+startY > y && offsetY+y+1 > heightNeg+startY && x+offsetX+startX >= 0 && x+offsetX+startX < width) {
                    ILubyte intensity = glyph->bitmap.buffer[(glyph->top-(y-heightNeg-startY)-1)*glyph->bitmap.pitch + x];
                    imageData[4*y*width + 4*(x+offsetX+startX) + 0] *= 1-intensity/255.f;
                    imageData[4*y*width + 4*(x+offsetX+startX) + 1] *= 1-intensity/255.f;
                    imageData[4*y*width + 4*(x+offsetX+startX) + 2] *= 1-intensity/255.f;
                    imageData[4*y*width + 4*(x+offsetX+startX) + 0] += ILubyte(intensity*fr);
                    imageData[4*y*width + 4*(x+offsetX+startX) + 1] += ILubyte(intensity*fg);
                    imageData[4*y*width + 4*(x+offsetX+startX) + 2] += ILubyte(intensity*fb);
                    imageData[4*y*width + 4*(x+offsetX+startX) + 3] = std::max(imageData[4*y*width + 4*(x+offsetX+startX) + 3], ILubyte(intensity * fa));
                }
            }
        }
        offsetX += glyph->bitmap.width;
        offsetX += uint(fontSize*space);
        if(text[i] == ' ') {offsetX += uint(fontSize*2*space);}
    }
    
    GLuint tex;
    glGenTextures(1, &tex);
    loadTexture(tex);
    
    if(imagePtr) {
        *imagePtr = image;
    } else {
        ilDeleteImages(1, &image);
    }
    
    return tex;
}

void ImageHelper::loadTexture(GLuint tex, ILuint * imagePtr) const {
    if(imagePtr) {ilBindImage(*imagePtr);}
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0,
            (ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL) == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE,
            ilGetData());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ImageHelper::paint(glm::vec4 color, glm::vec2 pos, float radius, ILuint * imagePtr, bool scaleHorizontally) {
    ilBindImage(*imagePtr);
    ILubyte * data = ilGetData();
    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    float scale = scaleHorizontally ? width : height;
    for(int x = std::max(0, (int)(pos.x*width-radius*scale)); x <= std::min(width-1, (int)(pos.x*width+radius*scale)); ++x) {
        for(int y = std::max(0, (int)(pos.y*height-radius*scale)); y <= std::min(height-1, (int)(pos.y*height+radius*scale)); ++y) {
            if((x-pos.x*width)*(x-pos.x*width) + (y-pos.y*height)*(y-pos.y*height) <= radius*radius*scale*scale) {
                data[4*y*width + 4*x + 0] = 255*color.r;
                data[4*y*width + 4*x + 1] = 255*color.g;
                data[4*y*width + 4*x + 2] = 255*color.b;
                data[4*y*width + 4*x + 3] = 255*color.a;
            }
        }
    }
}
void ImageHelper::paint(glm::vec4 color, glm::vec2 start, glm::vec2 end, float radius, ILuint * imagePtr, bool scaleHorizontally) {
    ilBindImage(*imagePtr);
    ILubyte * data = ilGetData();
    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    float scale = scaleHorizontally ? width : height;
    
    glm::vec2 size{width, height};
    start *= size;
    end *= size;
    radius *= scale;
    
    for(int x = std::max(0, (int)(std::min(start.x, end.x)-radius)); x <= std::min(width-1, (int)(std::max(start.x, end.x)+radius)); ++x) {
        for(int y = std::max(0, (int)(std::min(start.y, end.y)-radius)); y <= std::min(height-1, (int)(std::max(start.y, end.y)+radius)); ++y) {
            glm::vec2 point{x, y};
            bool circle0 = (x-start.x)*(x-start.x) + (y-start.y)*(y-start.y) <= radius*radius;
            bool circle1 = (x-  end.x)*(x-  end.x) + (y-  end.y)*(y-  end.y) <= radius*radius;
            bool rectangle = false;
            if(start != end) {
                glm::vec2 norm = glm::normalize(glm::vec2{start.y-end.y, end.x-start.x});
                rectangle = abs(glm::dot(point-start, norm)) <= radius && glm::dot(end-start, point-start) >= 0 && glm::dot(end-start, point-end) <= 0;
            }
            if(circle0 || circle1 || rectangle) {
                data[4*y*width + 4*x + 0] = 255*color.r;
                data[4*y*width + 4*x + 1] = 255*color.g;
                data[4*y*width + 4*x + 2] = 255*color.b;
                data[4*y*width + 4*x + 3] = 255*color.a;
            }
        }
    }
}

