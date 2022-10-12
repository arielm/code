#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float FOVY = 45;
static constexpr float CAMERA_DISTANCE = 400;
static constexpr float CAMERA_ELEVATION = 60;

void Sketch::setup()
{
    lineBatch
        .setPrimitive(GL_LINES)
        .setShader(colorShader)
        .setShaderColor(0, 0, 0, 0.5f);

    drawGrid(math::Rectf(-1000, -1000, 2000, 2000), glm::vec2(0), 20);

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
        .rotateX(CAMERA_ELEVATION * D2R)
        .translate(center + pan);

    State()
        .setShaderMatrix<MVP>(camera.getViewProjectionMatrix())
        .apply();

    // ---

    lineBatch.flush();
}

void Sketch::addTouch(int index, float x, float y)
{
    dragOrigin = convert(glm::vec2(x, y));
}

void Sketch::updateTouch(int index, float x, float y)
{
    pan = convert(glm::vec2(x, y)) - dragOrigin;
}

void Sketch::removeTouch(int index, float x, float y)
{
    center += pan;
    pan = glm::vec2(0);
}

void Sketch::drawGrid(const math::Rectf &bounds, const glm::vec2 &offset, float size)
{
    float ox1 = floorf(bounds.x1 / size);
    float ox2 = ceilf(bounds.x2 / size);

    float oy1 = floorf(bounds.y1 / size);
    float oy2 = ceilf(bounds.y2 / size);

    int nx = int(ox2 - ox1) + 1;
    int ny = int(oy2 - oy1) + 1;

    for (int ix = 0; ix < nx; ix++)
    {
        float xx = bounds.x1 + size * ix - math::boundf(offset.x, size);
        if ((xx >= bounds.x1) && (xx <= bounds.x2))
        {
            lineBatch.addVertices(glm::vec3(xx, bounds.y1, 0), glm::vec3(xx, bounds.y2, 0));
        }
    }

    for (int iy = 0; iy < ny; iy++)
    {
        float yy = bounds.y1 + size * iy - math::boundf(offset.y, size);
        if ((yy >= bounds.y1) && (yy <= bounds.y2))
        {
            lineBatch.addVertices(glm::vec3(bounds.x1, yy, 0), glm::vec3(bounds.x2, yy, 0));
        }
    }
}

glm::vec2 Sketch::convert(const glm::vec2 &position)
{
    auto ray = camera.getRay(position);
    auto result = ray.planeIntersection(glm::vec3(center, 0), glm::vec3(0, 0, 1));
    if (result.first)
    {
        return glm::vec2(ray.origin + result.second * ray.direction); // SIMPLIFICATION TO 2D IS POSSIBLE BECAUSE PLANE LIES ON GROUND (0)
    }

    return glm::vec2(0); // XXX
}
