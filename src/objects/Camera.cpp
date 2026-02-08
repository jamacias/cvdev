#include "Camera.h"

Camera::Camera(Scene3D& scene)
: Object3D(&scene)
, SceneGraph::Camera3D(*this)
{
}
