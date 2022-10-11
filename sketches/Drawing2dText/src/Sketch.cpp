#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

constexpr float PADDING = 20;

void Sketch::setup()
{
    font1 = fontManager.getFont(InputSource::resource("Helvetica_Regular_64.fnt"), XFont::Properties2d());
    font1->setShader(textureAlphaShader);
    font1->setSize(20);
    font1->setColor(0, 0, 0, 1);

    strokeBatch
        .setPrimitive(GL_LINES)
        .setShader(colorShader)
        .setShaderColor(0, 0, 0, 0.25f);

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
    strokeBatch.clear();
    drawGuides();

    drawText1();
}

void Sketch::draw()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

    Matrix modelViewMatrix;
    modelViewMatrix
        .translate(0, windowInfo.height)
        .scale(1, -1);

    gl::State()
        .setShaderMatrix(modelViewMatrix * projectionMatrix)
        .apply();

    // ---

    strokeBatch.flush();
    font1->replaySequence(sequence1);
}

void Sketch::drawGuides()
{
    float width = windowInfo.width;
    float height = windowInfo.height;

    strokeBatch.addVertices(glm::vec2(PADDING, 0), glm::vec2(PADDING, height));
    strokeBatch.addVertices(glm::vec2(width - PADDING, 0), glm::vec2(width - PADDING, height));
    strokeBatch.addVertices(glm::vec2(0, PADDING), glm::vec2(width, PADDING));
    strokeBatch.addVertices(glm::vec2(0, height - PADDING), glm::vec2(width, height - PADDING));
}

void Sketch::drawText1()
{
    font1->beginSequence(sequence1);

    drawText(*font1, u"bottom-left", PADDING, windowInfo.height - PADDING);

    drawAlignedText(*font1, u"top-left", glm::vec2(PADDING, PADDING), XFont::ALIGN_LEFT, XFont::ALIGN_TOP);
    drawAlignedText(*font1, u"top-right", glm::vec2(windowInfo.width - PADDING, PADDING), XFont::ALIGN_RIGHT, XFont::ALIGN_TOP);
    drawAlignedText(*font1, u"bottom-right", glm::vec2(windowInfo.width - PADDING, windowInfo.height - PADDING), XFont::ALIGN_RIGHT, XFont::ALIGN_BASELINE);
    drawAlignedText(*font1, u"top-middle", glm::vec2(windowInfo.width / 2, PADDING), XFont::ALIGN_MIDDLE, XFont::ALIGN_TOP);
    drawAlignedText(*font1, u"bottom-middle", glm::vec2(windowInfo.width / 2, windowInfo.height - PADDING), XFont::ALIGN_MIDDLE, XFont::ALIGN_BASELINE);

    Matrix matrix;
    matrix
        .translate(PADDING, windowInfo.height / 2)
        .rotateZ(-HALF_PI);
    drawAlignedText(*font1, u"middle-left", matrix, XFont::ALIGN_MIDDLE, XFont::ALIGN_TOP);

    matrix
        .setTranslate(windowInfo.width - PADDING, windowInfo.height / 2)
        .rotateZ(HALF_PI);
    drawAlignedText(*font1, u"middle-right", matrix, XFont::ALIGN_MIDDLE, XFont::ALIGN_TOP);

    font1->endSequence();
}

void Sketch::drawText(XFont &font, const u16string &text, float x, float y)
{
    for (auto c : text)
    {
        auto glyphIndex = font.getGlyphIndex(c);
        font.addGlyph(glyphIndex, x, y);
        x += font.getGlyphAdvance(glyphIndex);
    }
}

void Sketch::drawAlignedText(XFont &font, const u16string &text, const glm::vec2 &position, XFont::Alignment alignX, XFont::Alignment alignY)
{
    auto center = position + font.getOffset(text, alignX, alignY);
    drawText(font, text, center.x, center.y);
}

void Sketch::drawAlignedText(XFont &font, const u16string &text, Matrix &matrix, XFont::Alignment alignX, XFont::Alignment alignY)
{
    auto offset = font.getOffset(text, alignX, alignY);

    for (auto c : text)
    {
        auto glyphIndex = font.getGlyphIndex(c);
        font.addGlyph(matrix, glyphIndex, offset.x, offset.y);
        offset.x += font.getGlyphAdvance(glyphIndex);
    }
}
