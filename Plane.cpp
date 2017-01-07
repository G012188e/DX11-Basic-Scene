#include "Plane.h"
#include <iostream>

Plane::Plane(GameObject* _planeBody)
{
	planeBody = _planeBody;

	planePos = planeBody->GetTransform()->GetPosition();

	planeRotation = 0.0f;
	planeRotationSpeed = 0.001f;
	planeWheelRotation = 0.0f;

	engineSpeedAdd = 0.0008f;
}

Plane::~Plane()
{

}

void Plane::CalculateForwardVector()
{
	planeBody->GetTransform()->GetRotation();

	planeForwardVector.x = sin((planeRotation / 17.425f) * (XM_PI / 180.0f));
	planeForwardVector.y = 0.0f;
	planeForwardVector.z = cos((planeRotation / 17.425f) * (XM_PI / 180.0f));

	float planeDirectionMag = sqrt((planeForwardVector.x * planeForwardVector.x) + (planeForwardVector.y * planeForwardVector.y) + (planeForwardVector.z * planeForwardVector.z));
	planeForwardVector = XMFLOAT3((planeForwardVector.x / planeDirectionMag), (planeForwardVector.y / planeDirectionMag), (planeForwardVector.z / planeDirectionMag));

	PlaneParticleModel* planeBodyModel = (PlaneParticleModel*)planeBody->GetParticleModel();
	planeBodyModel->SetPlaneDirection(planeForwardVector);

	// Reset Car Rotation if over 360 or 0 degrees
	if (planeRotation <= -6247.0f || planeRotation >= 6247.0f)
	{
		planeRotation = 0.0f;
	}
}

void Plane::Update(float t)
{
	CalculateForwardVector();

	PlaneParticleModel* planeBodyModel = (PlaneParticleModel*)planeBody->GetParticleModel();
	float engineSpeed = planeBodyModel->GetEngineSpeed();

	// Rotate Car in consideration to the Wheels Rotation
	if (engineSpeed > 0)
	{
		planeRotation += (planeWheelRotation * 100) * (engineSpeed * 20);
	}
	else if (engineSpeed < 0)
	{
		engineSpeed *= -1;
		planeRotation -= (planeWheelRotation * 100) * (engineSpeed * 20);
	}

	planeBody->GetTransform()->SetRotation(0.0f, planeRotation * planeRotationSpeed, 0.0f);

	// Update Transform
	planeBody->Update(t);

	// Update Particle Model
	planeBody->GetParticleModel()->Update(t);
}

void Plane::Draw(ID3D11DeviceContext* _pImmediateContext)
{
	planeBody->Draw(_pImmediateContext);
}
