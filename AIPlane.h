#pragma once

#include "Plane.h"
#include "AiPlaneParticleModel.h"
#include "GameObject.h"

#include <directxmath.h>
#include <d3d11_1.h>

#include "Waypoint.h"

class AIPlane : public Plane
{
public:
	AIPlane(GameObject* _planeBody);
	~AIPlane();

	void Update(float t, XMFLOAT3 controledPlanePos);

	// ----------------- PATH FINDING -------------------- //
	void Pathfinding();

	// --------------- STEERING BEHAVIOUR ---------------- //
	void Seek(XMFLOAT3 target);
	void Flee(XMFLOAT3 target);
	void Arrive(XMFLOAT3 target);

	void Input();

private:
	// Path Finding
	vector < XMFLOAT3 > mRoute;

	int steeringState;

	bool mWaypointReached;

	XMFLOAT3 targetPosition;
	int targetIndex;

	XMFLOAT3 controlledPlanePos;
};

