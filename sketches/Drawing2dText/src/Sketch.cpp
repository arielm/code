#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

constexpr float PADDING = 20;
constexpr float RADIUS = 255;

const u16string TEXT = u"Some girls are bigger than others. Some girls mothers are bigger than other girls' mothers."; // Lyrics by the Smiths

void Sketch::setup()
{
    font1 = fontManager.getFont(InputSource::resource("Helvetica_Regular_64.fnt"), XFont::Properties2d());
    font1->setShader(textureAlphaShader);
    font1->setSize(20);
    font1->setColor(0, 0, 0, 1);

    font2 = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties2d());
    font2->setShader(textureAlphaShader);
    font2->setSize(40);
    font2->setColor(0.75f, 0, 0, 1);

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
    drawCircle(windowInfo.center(), RADIUS);

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

    font2->beginSequence(sequence2);
    drawCircularText(*font2, TEXT, windowInfo.center(), RADIUS, -clock()->getTime() * 100.0f, XFont::ALIGN_MIDDLE);
    font2->endSequence();
    font2->replaySequence(sequence2);
}

void Sketch::drawGuides()
{
    strokeBatch.addVertices(glm::vec2(PADDING, 0), glm::vec2(PADDING, windowInfo.height));
    strokeBatch.addVertices(glm::vec2(windowInfo.width - PADDING, 0), glm::vec2(windowInfo.width - PADDING, windowInfo.height));
    strokeBatch.addVertices(glm::vec2(0, PADDING), glm::vec2(windowInfo.width, PADDING));
    strokeBatch.addVertices(glm::vec2(0, windowInfo.height - PADDING), glm::vec2(windowInfo.width, windowInfo.height - PADDING));
}

void Sketch::drawCircle(const glm::vec2 &position, float radius)
{
    float segmentLength = 15;
    int n = ceilf(TWO_PI * radius / segmentLength) + 1;
    vector<glm::vec2> points;

    for (int i = 0; i < n; i++)
    {
        float d = i * segmentLength / radius;
        points.emplace_back(position.x + sinf(d) * radius, position.y - cosf(d) * radius);
    }

    for (int i = 0; i < n - 1; i++)
    {
        strokeBatch.addVertices(points[i], points[i + 1]);
    }
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
    auto newPosition = position + font.getOffset(text, alignX, alignY);
    drawText(font, text, newPosition.x, newPosition.y);
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

void Sketch::drawCircularText(chr::XFont &font, const std::u16string &text, const glm::vec2 &position, float radius, float offset, chr::XFont::Alignment alignY)
{
    float offsetY = font.getOffsetY(alignY);
    Matrix matrix;

    for (auto c : text)
    {
        auto glyphIndex = font.getGlyphIndex(c);
        float halfWidth = font.getGlyphAdvance(glyphIndex) / 2;
        offset += halfWidth;

        if (glyphIndex >= 0)
        {
            float d = offset / radius;
            matrix
                .setTranslate(position.x + sinf(d) * radius, position.y - cosf(d) * radius)
                .rotateZ(d);

            font.addGlyph(matrix, glyphIndex, -halfWidth, offsetY);
        }

        offset += halfWidth;
    }
}