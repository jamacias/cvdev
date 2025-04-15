#ifndef OBJECTS_GRID_H
#define OBJECTS_GRID_H

#include "../traits/traits.h"
#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/FlatGL.h>
#include <Magnum/SceneGraph/Camera.h>

using namespace Magnum;

class Grid : public Object3D, public SceneGraph::Drawable3D
{
public:
    explicit Grid(Object3D &parent, SceneGraph::DrawableGroup3D &drawables);
    void draw(const Matrix4& transformation, SceneGraph::Camera3D& camera);

private:
    Shaders::FlatGL3D shader_;
    GL::Mesh grid_;
};

#endif // OBJECTS_GRID_H