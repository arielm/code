#include "Sketch.h"

#include "chr/path/SplinePath.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;
using namespace path;

const u16string TEXT = u"Some girls are bigger than others. Some girls mothers are bigger than other girls' mothers."; // Lyrics by the Smiths

void Sketch::setup()
{
    font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties2d());
    font->setShader(textureAlphaShader);
    font->setSize(20);
    font->setColor(0, 0, 0, 1);

    lineBatch
        .setPrimitive(GL_LINES)
        .setShader(colorShader)
        .setShaderColor(0, 0, 0, 0.25f);

    // ---

    MatrixAffine matrix;
    matrix.scale(3);

    SplinePath<glm::vec2> peanut;
    peanut
        .setType(SplinePath<glm::vec2>::TYPE_BSPLINE)
        .setSamplingTolerance(16)
        .add(-100, -100)
        .add(   0,  -25)
        .add( 100, -100)
        .add( 200,    0)
        .add( 100,  100)
        .add(   0,   25)
        .add(-100,  100)
        .add(-200,    0)
        .close()
        .transformPoints(matrix);

    path
        .setMode(FollowablePath2D::MODE_LOOP)
        .begin()
        .add(peanut.getPolyline())
        .end();

    drawPolyline(path.getPoints());

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
        .scale(1, -1);

    gl::State()
        .setShaderMatrix(modelViewMatrix * projectionMatrix)
        .apply();

    // ---

    lineBatch.flush();

    font->beginSequence(sequence);
    drawTextOnPath(*font, TEXT, path, -clock()->getTime() * 75.0f, XFont::ALIGN_MIDDLE);
    font->endSequence();
    font->replaySequence(sequence);
}

void Sketch::drawPolyline(const vector<FollowablePath2D::Point> &points)
{
    auto size = points.size();
    if (size > 1)
    {
        for (auto i = 0; i < size - 1; i++)
        {
            lineBatch.addVertices(points[i].position, points[i + 1].position);
        }
    }
}

void Sketch::drawTextOnPath(XFont &font, const u16string &text, const FollowablePath2D &path, float offset, XFont::Alignment alignY)
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
            path
                .offsetToValue(offset, halfWidth * 2)
                .applyToMatrix(matrix);

            font.addGlyph(matrix, glyphIndex, -halfWidth, offsetY);
        }

        offset += halfWidth;
    }
}
