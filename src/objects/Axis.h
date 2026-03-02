#ifndef OBJECTS_AXIS_H
#define OBJECTS_AXIS_H

#include "../traits/traits.h"
#include "Magnum/Shaders/Shaders.h"

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/VertexColorGL.h>

using namespace Magnum;

class Axis : public Object3D, public SceneGraph::Drawable3D
{
public:
    explicit Axis(Object3D* parent, SceneGraph::DrawableGroup3D* drawables);

    void draw(const Matrix4& transformation, SceneGraph::Camera3D& camera);

private:
    Shaders::VertexColorGL3D shader_;
    GL::Mesh                 axis_;
};

#endif // OBJECTS_AXIS_H
