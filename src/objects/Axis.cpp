#include "Axis.h"

#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/Trade/MeshData.h>

Axis::Axis(Object3D& parent, SceneGraph::DrawableGroup3D& drawables)
: SceneGraph::Drawable3D(parent, &drawables)
{
    using namespace Math::Literals;

    axis_ = MeshTools::compile(Primitives::axis3D());
}

void Axis::draw(const Matrix4& transformation, SceneGraph::Camera3D& camera)
{
    using namespace Math::Literals;

    shader_.setTransformationProjectionMatrix(camera.projectionMatrix() * transformation).draw(axis_);
}
