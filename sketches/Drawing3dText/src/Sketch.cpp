#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float FOVY = 45;
static constexpr float CAMERA_DISTANCE = 180;
static constexpr float CAMERA_ELEVATION = -85;

static constexpr float R = 50;
static constexpr float TURNS = 10;
static constexpr float H = 100;

void Sketch::setup()
{
    auto lines = utils::readLines<u16string>(InputSource::resource("song.txt")); // Lyrics by Genesis

    u16string text;
    for (const auto &line : lines)
    {
        text += line + u" ";
    }

    font = fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties3d());
    font->setShader(textureAlphaShader);
    font->setSize(8);

    float L = TWO_PI * TURNS * R;
    float offset = (L - font->getStringAdvance(text)) / 2;

    font->beginSequence(sequence);
    drawTextHelix(*font, text, R, TURNS, -H, offset, XFont::ALIGN_MIDDLE);
    font->endSequence();

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
    camera
        .setFov(FOVY)
        .setClip(1, 1000)
        .setWindowSize(windowInfo.size);
}

void Sketch::draw()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    camera.getViewMatrix()
        .setIdentity()
        .scale(1, -1, 1)
        .translate(0, 0, -CAMERA_DISTANCE)
        .rotateX(-CAMERA_ELEVATION * D2R)
        .rotateZ(clock()->getTime() * 0.5f)
        .translate(0, 0, H / 2);

    State()
        .setShaderMatrix<MVP>(camera.getViewProjectionMatrix())
        .apply();

    // ---

    glCullFace(GL_FRONT);
    font->setColor(1, 1, 1, 0.333f);
    font->replaySequence(sequence);

    glCullFace(GL_BACK);
    font->setColor(1, 1, 1, 1);
    font->replaySequence(sequence);
}

void Sketch::drawTextHelix(XFont &font, const u16string &text, float r, float turns, float h, float offset, XFont::Alignment alignY)
{
    float l = TWO_PI * turns;
    float L = l * r;
    float dz = h / l;
    float ay = -atan2f(h, L);

    float offsetY = font.getOffsetY(alignY);
    Matrix matrix;

    for (auto c : text)
    {
        auto glyphIndex = font.getGlyphIndex(c);
        float halfWidth = font.getGlyphAdvance(glyphIndex) / 2;
        offset += halfWidth;

        if (glyphIndex >= 0)
        {
            float d = offset / r;
            matrix
                .setTranslate(-sinf(-d) * r, +cosf(-d) * r, d * dz)
                .rotateZ(-d)
                .rotateY(ay)
                .rotateX(-HALF_PI);

            font.addGlyph(matrix, glyphIndex, -halfWidth, offsetY);
        }

        offset += halfWidth;
    }
}
