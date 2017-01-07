#pragma once

#include "Plane.h"
#include "PlaneParticleModel.h"
#include "GameObject.h"

#include <directxmath.h>
#include <d3d11_1.h>

class ControlledPlane : public Plane
{
public:
	ControlledPlane(GameObject* _planeBody);
	~ControlledPlane();

	void Update(float t);
	
	// Plane Input
	void Input();

	float engineSpeedAdd;
private:

};