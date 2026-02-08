#ifndef TRAITS_TRAITS_H
#define TRAITS_TRAITS_H

#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>

using namespace Magnum;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D>  Scene3D;

#endif // TRAITS_TRAITS_H
