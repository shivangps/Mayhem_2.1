#pragma once
#include "DXMeshSystem.h"
#include "DXTextureSystem.h"
#include "Camera.h"

class DXRenderComponent
{
public:
	DXRenderComponent(Transform* transform) : transform(transform) {}

	Transform* transform = nullptr;


};