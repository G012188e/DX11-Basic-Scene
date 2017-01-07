#include "ControlledPlane.h"
#include <iostream>

ControlledPlane::ControlledPlane(GameObject* _planeBody) : Plane(_planeBody)
{
	engineSpeedAdd = 0.0008f;
}

ControlledPlane::~ControlledPlane()
{

}

void ControlledPlane::Input()
{
	PlaneParticleModel* planeBodyModel = (PlaneParticleModel*)GetPlaneBody()->GetParticleModel();
	float engineSpeed = planeBodyModel->GetEngineSpeed();

	XMFLOAT3 planePos = GetPlaneBody()->GetTransform()->GetPosition();

	if (GetAsyncKeyState('B'))
	{
		planePos = XMFLOAT3(planePos.x, 20.0f, planePos.z);
		GetPlaneBody()->GetTransform()->SetPosition(planePos);
	}

	if (GetAsyncKeyState('W'))
	{
		// Set Car Engine Speed
		planeBodyModel->AddEngineSpeed((engineSpeedAdd));
	}
	else if (GetAsyncKeyState('S'))
	{
		if (engineSpeed > 0)
		{
			planeBodyModel->AddEngineSpeed(-engineSpeedAdd * 2.0f);
		}
		else
		{
			planeBodyModel->AddEngineSpeed((-engineSpeedAdd / 1.5f));
		}
	}
	else
	{
		float engineSpeed = planeBodyModel->GetEngineSpeed();

		if (engineSpeed > 0)
		{
			planeBodyModel->AddEngineSpeed(-engineSpeedAdd);

			if (engineSpeed < 0.0008f && engineSpeed != 0.0f)
			{
				planeBodyModel->SetEngineSpeed(0.0f);
			}
		}
		else if (engineSpeed < 0)
		{
			planeBodyModel->AddEngineSpeed(engineSpeedAdd);
		}
	}

	// Car Rotation Check
	if (GetAsyncKeyState('A'))
	{
		AddPlaneWheelRotation(-0.01f);
	}
	else if (GetAsyncKeyState('D'))
	{
		AddPlaneWheelRotation(0.01f);
	}
	else
	{
		if (GetPlaneWheelRotation() < 0)
		{
			AddPlaneWheelRotation(0.02f);
		}
		else if (GetPlaneWheelRotation() > 0)
		{
			AddPlaneWheelRotation(-0.02f);
		}

		if (GetPlaneWheelRotation() < 0.01f && GetPlaneWheelRotation() > -0.01f)
		{
			SetPlaneWheelRotation(0);
		}
	}
}

void ControlledPlane::Update(float t)
{
	PlaneParticleModel* planeBodyModel = (PlaneParticleModel*)GetPlaneBody()->GetParticleModel();
	Transform* transform = (Transform*)GetPlaneBody()->GetTransform();

	planeBodyModel->SetOrigin(transform->GetPosition());
	float engineSpeed = planeBodyModel->GetEngineSpeed();

	XMFLOAT3 velTemp = GetPlaneBody()->GetParticleModel()->GetVelocity();
	XMFLOAT3 carVelTemp = planeBodyModel->GetPlaneVelocity();

	velTemp.x += carVelTemp.x;
	velTemp.y += carVelTemp.y;
	velTemp.z += carVelTemp.z;

	planeBodyModel->SetVelocity(velTemp);

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

