#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/xf/FontManager.h"

class Sketch : public chr::CrossSketch
{
public:
    void setup() final;
    void resize();
    void draw() final;

protected:
    chr::gl::VertexBatch<chr::gl::XYZ> strokeBatch;
    chr::gl::shaders::ColorShader colorShader;

    chr::xf::FontManager fontManager;
    std::shared_ptr<chr::XFont> font1;
    chr::xf::FontSequence sequence1;
    chr::gl::shaders::TextureAlphaShader textureAlphaShader;

    void drawGuides();
    void drawText1();

    static void drawText(chr::XFont &font, const std::u16string &text, float x, float y);
    static void drawAlignedText(chr::XFont &font, const std::u16string &text, const glm::vec2 &position, chr::XFont::Alignment alignX, chr::XFont::Alignment alignY);
    static void drawAlignedText(chr::XFont &font, const std::u16string &text, chr::gl::Matrix &matrix, chr::XFont::Alignment alignX, chr::XFont::Alignment alignY);
};
