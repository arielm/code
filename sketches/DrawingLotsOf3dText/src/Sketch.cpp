#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float FONT_SIZE = 12;
static constexpr float GUTTER = 12;

void Sketch::setup()
{
    vector<vector<u16string>> songs;
    for (auto &name : {"song1.txt", "song2.txt", "song3.txt", "song4.txt", "song5.txt", "song6.txt", "song7.txt"}) // Lyrics by the Smiths
    {
        songs.push_back(utils::readLines<u16string>(InputSource::resource(name)));
    }

    font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties3d());
    font->setShader(textureAlphaShader);
    font->setSize(FONT_SIZE);
    font->setColor(0, 0, 0, 1);

    lineBatch
        .setPrimitive(GL_LINES)
        .setShader(colorShader)
        .setShaderColor(0, 0, 0, 0.33f);

    // ---

    font->beginSequence(sequence);

    float x = -150;
    float y= -70;
    float maxHeight = 0;
    vector<float> xx;
    for (const auto &song : songs)
    {
        auto result = drawLines(*font, song, x, y);
        x += result.x + GUTTER;
        xx.push_back(x);
        x += GUTTER;

        if (result.y > maxHeight) maxHeight = result.y;
    }

    font->endSequence();

    float yy = y - font->getAscent();
    for (int i = 0; i < xx.size() -1; i++)
    {
        lineBatch.addVertices(glm::vec2(xx[i], yy), glm::vec2(xx[i], yy + maxHeight));
    }

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
    camera
        .setFov(45)
        .setClip(1, 10000)
        .setWindowSize(windowInfo.size);
}

void Sketch::draw()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    camera.getViewMatrix()
        .setIdentity()
        .scale(1, -1, 1)
        .translate(0, 0, -400)
        .rotateX(45 * D2R)
        .rotateZ(-15 * D2R);

    Matrix panMatrix;
    panMatrix.translate(pan);

    State()
        .setShaderMatrix<MVP>(panMatrix * camera.getViewProjectionMatrix())
        .apply();

    // ---

    lineBatch.flush();
    font->replaySequence(sequence);
}

void Sketch::addTouch(int index, float x, float y)
{
    dragOrigin = convert(glm::vec2(x, y)) - pan;
}

void Sketch::updateTouch(int index, float x, float y)
{
    pan = convert(glm::vec2(x, y)) - dragOrigin;
}

glm::vec2 Sketch::drawLines(XFont &font, const vector<u16string> &lines, float x, float y)
{
    float lineHeight = font.getHeight() * 1.2f;
    float maxWidth = 0;
    for (const auto &line : lines)
    {
        float width = font.getStringAdvance(line);
        if (width > maxWidth) maxWidth = width;

        drawText(font, line, x, y);
        y += lineHeight;
    }

    return glm::vec2(maxWidth, lineHeight * lines.size());
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

glm::vec2 Sketch::convert(const glm::vec2 &position)
{
    auto ray = camera.getRay(position);
    auto result = ray.planeIntersection(glm::vec3(0), glm::vec3(0, 0, 1));
    if (result.first)
    {
        return glm::vec2(ray.origin + result.second * ray.direction); // Simplification to 2d is possible because plane lies on ground (0)
    }

    return glm::vec2(0);
}
