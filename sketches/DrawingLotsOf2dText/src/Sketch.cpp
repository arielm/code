#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

void Sketch::setup()
{
    vector<u16string> songs;
    for (auto &name : {"song1.txt", "song2.txt", "song3.txt", "song4.txt", "song5.txt", "song6.txt", "song7.txt", "song8.txt"}) // Lyrics by the Smiths
    {
        auto lines = utils::readLines<u16string>(InputSource::resource(name));

        u16string text;
        for (const auto &line : lines)
        {
            text += line + u" ";
        }

        songs.push_back(text);
    }

    font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties2d());
    font->setShader(textureAlphaShader);
    font->setSize(16);

    // ---

    font->beginSequence(sequence, true);

    float offset = 0;
    bool red = false;
    for (const auto &song : songs)
    {
        glm::vec4 color = red ? glm::vec4(0.75f, 0, 0, 1) : glm::vec4(0, 0, 0, 1);
        font->setColor(color);

        offset = drawTextSpiral(*font, song, 30, 900, 45, offset, XFont::ALIGN_MIDDLE);
        red ^= true;
    }

    font->endSequence();

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::draw()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

    Matrix modelViewMatrix;
    modelViewMatrix
        .translate(windowInfo.center())
        .scale(1, -1)
        .rotateZ(clock()->getTime() * 0.33f);

    gl::State()
        .setShaderMatrix(modelViewMatrix * projectionMatrix)
        .apply();

    // ---

    font->replaySequence(sequence);
}

float Sketch::drawTextSpiral(XFont &font, const u16string &text, float r1, float r2, float turns, float offset, chr::XFont::Alignment alignY)
{
    float l = TWO_PI * turns;
    float dr = (r2 - r1) / l;

    float offsetY = font.getOffsetY(alignY);
    Matrix matrix;

    for (auto c : text)
    {
        auto glyphIndex = font.getGlyphIndex(c);
        float halfWidth = font.getGlyphAdvance(glyphIndex) / 2;
        offset += halfWidth;

        if (glyphIndex >= 0)
        {
            float r = sqrtf(r1 * r1 + 2 * dr * offset);
            float d = (r - r1) / dr;

            matrix
                .setTranslate(-sinf(-d) * r, +cosf(-d) * r)
                .rotateZ(-d);

            font.addGlyph(matrix, glyphIndex, -halfWidth, offsetY);
        }

        offset += halfWidth;
    }

    return offset;
}