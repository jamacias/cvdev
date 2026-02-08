#include "Grid.h"

#include <Magnum/Primitives/Grid.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/MeshData.h>

Grid::Grid(Object3D& parent, SceneGraph::DrawableGroup3D& drawables)
: SceneGraph::Drawable3D(parent, &drawables)
{
    using namespace Math::Literals;

    grid_ = MeshTools::compile(Primitives::grid3DWireframe({15, 15}));

    rotateX(90.0_degf).scale(Vector3{8.0f});
}

void Grid::draw(const Matrix4& transformation, SceneGraph::Camera3D& camera)
{
    using namespace Math::Literals;

    shader_.setColor(0x747474_rgbf)
        .setTransformationProjectionMatrix(camera.projectionMatrix() * transformation)
        .draw(grid_);
}
