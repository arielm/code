#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/xf/FontManager.h"

class Sketch : public chr::CrossSketch
{
public:
    void setup() final;
    void draw() final;

protected:
    chr::xf::FontManager fontManager;
    std::shared_ptr<chr::XFont> font;
    chr::xf::FontSequence sequence;
    chr::gl::shaders::TextureAlphaShader textureAlphaShader;

    static float drawTextSpiral(chr::XFont &font, const std::u16string &text, float r1, float r2, float turns, float offset, chr::XFont::Alignment alignY);
};
