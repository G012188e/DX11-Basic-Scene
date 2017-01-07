#pragma once

#include "PlaneParticleModel.h"
#include "GameObject.h"

class Plane
{
public:
	Plane(GameObject* _planeBody);
	~Plane();

	virtual void Update(float t);
	void Draw(ID3D11DeviceContext* _pImmediateContext);

	// Get Plane Body
	GameObject* GetPlaneBody() const { return planeBody; };

	// Get/Set Plane Position
	XMFLOAT3 GetPlanePosition() const { return planePos; };
	void SetPlanePosition(XMFLOAT3 _planePos) { planePos = _planePos; };
	void SetPlanePosition(float x, float y, float z) { planePos.x = x, planePos.y = y, planePos.z = z; };

	// Get/Calculate Plane Forward Vector
	XMFLOAT3 GetForwardVector() const { return planeForwardVector; };
	void CalculateForwardVector();

	// Get/Set Plane Rotation
	float GetPlaneRotation() const { return planeRotation; };
	void SetPlaneRotation(float _planeRotation) { planeRotation = _planeRotation; planeRotation = planeRotation * (XM_PI / 180);
											  planeBody->GetTransform()->SetRotation(0.0f, planeRotation, 0.0f); };
	void AddPlaneRotation(float _planeRotation) { planeRotation = _planeRotation; planeRotation = planeRotation * (XM_PI / 180);
											  planeBody->GetTransform()->SetRotation(0.0f, planeRotation, 0.0f); };

	// Get/Set Plane Rotation Speed
//	float GetPlaneRotationSpeed() const { return planeRotationSpeed; };
//	void SetPlaneRotationSpeed(float _planeRotationSpeed) { planeRotationSpeed = planeRotationSpeed; };

	// Get/Set Plane Wheel Rotation
	float GetPlaneWheelRotation() const { return planeWheelRotation; };
	void SetPlaneWheelRotation(float _planeWheelRotation) { planeWheelRotation = _planeWheelRotation; };
	void AddPlaneWheelRotation(float _planeWheelRotation) { planeWheelRotation += _planeWheelRotation; };

private:
	// Plane Object
	GameObject* planeBody;

	// Plane Properties
	XMFLOAT3 planePos;
	XMFLOAT3 planeForwardVector;

	float planeRotation;
	float planeRotationSpeed;
	float planeWheelRotation;

	float engineSpeedAdd;
};