#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/path/FollowablePath2D.h"

#include "common/xf/FontManager.h"

class Sketch : public chr::CrossSketch
{
public:
    void setup() final;
    void draw() final;

protected:
    chr::gl::VertexBatch<chr::gl::XYZ> lineBatch;
    chr::gl::shaders::ColorShader colorShader;

    chr::xf::FontManager fontManager;
    std::shared_ptr<chr::XFont> font;
    chr::xf::FontSequence sequence;
    chr::gl::shaders::TextureAlphaShader textureAlphaShader;

    chr::path::FollowablePath2D path;

    void drawPolyline(const std::vector<chr::path::FollowablePath2D::Point> &points);
    static void drawTextOnPath(chr::XFont &font, const std::u16string &text, const chr::path::FollowablePath2D &path, float offset, chr::XFont::Alignment alignY);
};
