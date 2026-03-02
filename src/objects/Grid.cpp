#include "Grid.h"

#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Grid.h>
#include <Magnum/Trade/MeshData.h>

Grid::Grid(Object3D& parent, SceneGraph::DrawableGroup3D& drawables)
: SceneGraph::Drawable3D(parent, &drawables)
, origin_(parent, drawables)
{
    grid_ = MeshTools::compile(Primitives::grid3DWireframe({sizeMeters_ - 1, sizeMeters_ - 1}));
}

void Grid::draw(const Matrix4& transformation, SceneGraph::Camera3D& camera)
{
    using namespace Math::Literals;

    shader_.setColor(0x747474_rgbf)
        .setTransformationProjectionMatrix(camera.projectionMatrix() * transformation * Matrix4::scaling(Vector3{sizeMeters_ / 2.0f}))
        .draw(grid_);
}
