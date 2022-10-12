#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/Camera.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/xf/FontManager.h"

class Sketch : public chr::CrossSketch
{
public:
    void setup() final;
    void resize() final;
    void draw() final;

    void addTouch(int index, float x, float y) final;
    void updateTouch(int index, float x, float y) final;
    void removeTouch(int index, float x, float y) final;

protected:
    chr::gl::Camera camera;

    chr::gl::VertexBatch<chr::gl::XYZ> lineBatch;
    chr::gl::shaders::ColorShader colorShader;

    glm::vec2 dragOrigin;
    glm::vec2 center, pan;

    chr::xf::FontManager fontManager;
    std::shared_ptr<chr::xf::Font> font;
    chr::xf::FontSequence sequence;
    chr::gl::shaders::TextureAlphaShader textureAlphaShader;

    void drawGrid(const chr::math::Rectf &bounds, const glm::vec2 &offset, float size);
    glm::vec2 convert(const glm::vec2 &position);
};
