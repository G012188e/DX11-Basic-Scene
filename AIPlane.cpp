#include "AIPlane.h"

#include "WaypointManager.h"

AIPlane::AIPlane (GameObject* _planeBody) : Plane(_planeBody)
{
	targetPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	targetIndex = 0;

	steeringState = 1;

	PlaneParticleModel* planeBodyModel = (PlaneParticleModel*)GetPlaneBody()->GetParticleModel();
	planeBodyModel->SetEngineSpeedLimit(0.2f);

	planeBodyModel->SetUseConstAccel(false);

	mRoute.clear();
	mRoute.push_back(WaypointManager::Instance()->GetWaypointWithID(0)->GetPosition());
	mRoute.push_back(WaypointManager::Instance()->GetWaypointWithID(1)->GetPosition());
	mRoute.push_back(WaypointManager::Instance()->GetWaypointWithID(2)->GetPosition());
	mRoute.push_back(WaypointManager::Instance()->GetWaypointWithID(3)->GetPosition());
}

AIPlane::~AIPlane()
{

}

void AIPlane::Update(float t, XMFLOAT3 controledPlanePos)
{
	XMFLOAT3 planePos = GetPlaneBody()->GetTransform()->GetPosition();
	XMFLOAT3 velTemp = GetPlaneBody()->GetParticleModel()->GetVelocity();

	controlledPlanePos = controledPlanePos;

	if (steeringState == 2)
	Flee(controlledPlanePos);
	if (steeringState == 1)
	{
		Pathfinding();
		Seek(targetPosition);
	}

	PlaneParticleModel* planeBodyModel = (PlaneParticleModel*)GetPlaneBody()->GetParticleModel();
	float engineSpeed = planeBodyModel->GetEngineSpeed();

	// AI positioning
	XMFLOAT3 planeVelocity = planeBodyModel->GetPlaneVelocity();
	planePos = XMFLOAT3((planePos.x + planeVelocity.x), (planePos.y + planeVelocity.y), (planePos.z + planeVelocity.z));
	GetPlaneBody()->GetTransform()->SetPosition(planePos);

	// Limit Wheel Rotation
	float planeWheelLimit = 0.1f;

	if (GetPlaneWheelRotation() <= -planeWheelLimit)
	{
		SetPlaneWheelRotation(-planeWheelLimit);
	}
	else if (GetPlaneWheelRotation() >= planeWheelLimit)
	{
		SetPlaneWheelRotation(planeWheelLimit);
	}

	Plane::Update(t);
}

// ----------------- PATH FINDING -------------------- //

void AIPlane::Pathfinding()
{
	targetPosition = mRoute.at(targetIndex);
	XMFLOAT3 planePosition = GetPlaneBody()->GetTransform()->GetPosition();
	bool finishedPath = false;

	targetPosition.y = planePosition.y;

	// Distance to Target Position
	XMFLOAT3 targetDistance;
	targetDistance.x = targetPosition.x - planePosition.x;
	targetDistance.y = targetPosition.y - planePosition.y;
	targetDistance.z = targetPosition.z - planePosition.z;

	float targetDistanceMagnitude = sqrt((targetDistance.x * targetDistance.x) + (targetDistance.y * targetDistance.y) + (targetDistance.z * targetDistance.z));

	if (abs(targetDistanceMagnitude) >= 30.0f)
	{
		mWaypointReached = false;
	}

	if (abs(targetDistanceMagnitude) <= 30.0f && mWaypointReached == false)
	{
		mWaypointReached = true;
	}

	//Last node decision
	if ((targetIndex == (mRoute.size() - 1)) && mWaypointReached == true)
	{
		targetIndex = 0;
		finishedPath = true;
	}

	// Incrment to next node if not fiNshed route
	if ((mWaypointReached == true) && (finishedPath == false))
	{
		targetIndex++;
	}
}

// ------------------- INPUT ---------------------- //

void AIPlane::Input()
{
	if (GetAsyncKeyState('P'))
	{
		steeringState = 1;
	}

	if (GetAsyncKeyState('O'))
	{
		steeringState = 2;
	}
}

// --------------- STEERING BEHAVIOUR ---------------- //

void AIPlane::Seek(XMFLOAT3 target)
{
	XMFLOAT3 PlaneForwardVector = GetForwardVector();
	XMFLOAT3 planePos = GetPlaneBody()->GetTransform()->GetPosition();

	XMFLOAT3 upDirection = XMFLOAT3(planePos.x, planePos.y + 10.0f, planePos.z);
	upDirection.x = upDirection.x - planePos.x;
	upDirection.y = upDirection.y - planePos.y;
	upDirection.z = upDirection.z - planePos.z;

	XMFLOAT3 forwardDirection;
	forwardDirection.x = planePos.x - (planePos.x + PlaneForwardVector.x);
	forwardDirection.y = planePos.y - (planePos.y + PlaneForwardVector.y);
	forwardDirection.z = planePos.z - (planePos.z + PlaneForwardVector.z);

	XMFLOAT3 targetDirection;
	targetDirection.x = target.x - planePos.x;
	targetDirection.y = target.y - planePos.y;
	targetDirection.z = target.z - planePos.z;

	XMFLOAT3 crossProduct;
	crossProduct.x = (forwardDirection.y * targetDirection.z) - (forwardDirection.z * targetDirection.y);
	crossProduct.y = (forwardDirection.z * targetDirection.x) - (forwardDirection.x * targetDirection.z);
	crossProduct.z = (forwardDirection.x * targetDirection.y) - (forwardDirection.y * targetDirection.x);

	float dotProduct = (crossProduct.x + upDirection.x) + (crossProduct.y + upDirection.y) + (crossProduct.z + upDirection.z);

	AiPlaneParticleModel* carBodyTemp = (AiPlaneParticleModel*)GetPlaneBody()->GetParticleModel();
	float engineSpeed = carBodyTemp->GetEngineSpeed();

	if (dotProduct < 5.0f && dotProduct > -5.0f)
	{
		carBodyTemp->AddEngineSpeed(0.00008f);
		SetPlaneWheelRotation(0.0f);
	}
	else if (dotProduct > 5.0f)
	{
		AddPlaneWheelRotation(-0.0005f);

		if (engineSpeed < 0.1)
		{
			carBodyTemp->AddEngineSpeed(0.00002f);
		}
		else if (engineSpeed > 0.1)
		{
			carBodyTemp->AddEngineSpeed(-0.00002f);
		}
	}
	else if (dotProduct < 5.0f)
	{
		AddPlaneWheelRotation(0.0005f);

		if (engineSpeed < 0.1)
		{
			carBodyTemp->AddEngineSpeed(0.00002f);
		}
		else if (engineSpeed > 0.1)
		{
			carBodyTemp->AddEngineSpeed(-0.00002f);
		}
	}

	engineSpeed = carBodyTemp->GetEngineSpeed();

	GameObject* cartemp = GetPlaneBody();
	float carRotation = GetPlaneBody()->GetTransform()->GetRotation().y;
	float carWheelRotation = GetPlaneWheelRotation();
	float carRotationSpeed = 0.1f;

	//// Rotate Car in consideration to the Wheels Rotation
	//if (engineSpeed > 0)
	//{
	//	carRotation += (carWheelRotation * 100) * (engineSpeed * 20);
	//}
	//else if (engineSpeed < 0)
	//{
	//	engineSpeed *= -1;
	//	carRotation -= (carWheelRotation * 100) * (engineSpeed * 20);
	//}
}

void AIPlane::Flee(XMFLOAT3 target)
{
	XMFLOAT3 PlaneForwardVector = GetForwardVector();
	XMFLOAT3 planePos = GetPlaneBody()->GetTransform()->GetPosition();

	XMFLOAT3 upDirection = XMFLOAT3(planePos.x, planePos.y + 10.0f, planePos.z);
	upDirection.x = upDirection.x - planePos.x;
	upDirection.y = upDirection.y - planePos.y;
	upDirection.z = upDirection.z - planePos.z;

	XMFLOAT3 forwardDirection;
	forwardDirection.x = planePos.x - (planePos.x - PlaneForwardVector.x);
	forwardDirection.y = planePos.y - (planePos.y - PlaneForwardVector.y);
	forwardDirection.z = planePos.z - (planePos.z - PlaneForwardVector.z);

	XMFLOAT3 targetDirection;
	targetDirection.x = target.x - planePos.x;
	targetDirection.y = target.y - planePos.y;
	targetDirection.z = target.z - planePos.z;

	XMFLOAT3 crossProduct;
	crossProduct.x = (forwardDirection.y * targetDirection.z) - (forwardDirection.z * targetDirection.y);
	crossProduct.y = (forwardDirection.z * targetDirection.x) - (forwardDirection.x * targetDirection.z);
	crossProduct.z = (forwardDirection.x * targetDirection.y) - (forwardDirection.y * targetDirection.x);

	float dotProduct = (crossProduct.x + upDirection.x) + (crossProduct.y + upDirection.y) + (crossProduct.z + upDirection.z);

	AiPlaneParticleModel* carBodyTemp = (AiPlaneParticleModel*)GetPlaneBody()->GetParticleModel();
	float engineSpeed = carBodyTemp->GetEngineSpeed();

	if (dotProduct < 5.0f && dotProduct > -5.0f)
	{
		carBodyTemp->AddEngineSpeed(0.00008f);
		SetPlaneWheelRotation(0.0f);
	}
	else if (dotProduct > 5.0f)
	{
		AddPlaneWheelRotation(-0.0005f);

		if (engineSpeed < 0.1)
		{
			carBodyTemp->AddEngineSpeed(0.00002f);
		}
		else if (engineSpeed > 0.1)
		{
			carBodyTemp->AddEngineSpeed(-0.00002f);
		}
	}
	else if (dotProduct < 5.0f)
	{
		AddPlaneWheelRotation(0.0005f);

		if (engineSpeed < 0.1)
		{
			carBodyTemp->AddEngineSpeed(0.00002f);
		}
		else if (engineSpeed > 0.1)
		{
			carBodyTemp->AddEngineSpeed(-0.00002f);
		}
	}

	engineSpeed = carBodyTemp->GetEngineSpeed();

	GameObject* cartemp = GetPlaneBody();
	float carRotation = GetPlaneBody()->GetTransform()->GetRotation().y;
	float carWheelRotation = GetPlaneWheelRotation();
	float carRotationSpeed = 0.1f;
}