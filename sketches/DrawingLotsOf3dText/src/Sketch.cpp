#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float FOVY = 45;
static constexpr float CAMERA_DISTANCE = 400;
static constexpr float CAMERA_ELEVATION = 60;

void Sketch::setup()
{
    auto lines = utils::readLines<u16string>(InputSource::resource("song1.txt"));

    font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties3d());
    font->setShader(textureAlphaShader);
    font->setSize(12);
    font->setColor(0, 0, 0, 1);

    font->beginSequence(sequence);
    drawLines(*font, lines, 0, 0);
    font->endSequence();

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
    camera
        .setFov(FOVY)
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
        .translate(0, 0, -CAMERA_DISTANCE)
        .rotateX(CAMERA_ELEVATION * D2R);

    Matrix panMatrix;
    panMatrix.translate(pan);

    State()
        .setShaderMatrix<MVP>(panMatrix * camera.getViewProjectionMatrix())
        .apply();

    // ---

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

void Sketch::drawLines(XFont &font, const vector<u16string> &lines, float x, float y)
{
    float lineHeight = font.getHeight() * 1.2f;

    for (const auto &line : lines)
    {
        drawText(font, line, x, y);
        y += lineHeight;
    }
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
        return glm::vec2(ray.origin + result.second * ray.direction); // SIMPLIFICATION TO 2D IS POSSIBLE BECAUSE PLANE LIES ON GROUND (0)
    }

    return glm::vec2(0);
}
