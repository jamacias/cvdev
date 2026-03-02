#ifndef TRAITS_TRAITS_H
#define TRAITS_TRAITS_H

#include <Magnum/SceneGraph/RigidMatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>

using namespace Magnum;
typedef SceneGraph::Object<SceneGraph::RigidMatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::RigidMatrixTransformation3D>  Scene3D;

#endif // TRAITS_TRAITS_H
