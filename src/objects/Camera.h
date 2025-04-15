#ifndef OBJECTS_CAMERA_H
#define OBJECTS_CAMERA_H

#include "../traits/traits.h"
#include <Magnum/SceneGraph/Camera.h>

using namespace Magnum;

class Camera : public Object3D, public SceneGraph::Camera3D
{
public:
    explicit Camera(Scene3D &scene);
};

#endif // OBJECTS_CAMERA_H