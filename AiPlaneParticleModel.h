#pragma once

#include "ParticleModel.h"

class AiPlaneParticleModel : public ParticleModel
{
public:
	AiPlaneParticleModel(Transform* transform, float mass);
	~AiPlaneParticleModel();

	void CalculateWheelSpeed();
	void CalculateThrust(float t);
	void CalculateVelocity();

	void TruncateSpeed();

	void Update(float t);

	// --------------- Get/Set ------------------ //

	// Get/Set Car Direction
	XMFLOAT3 GetCarDirection() const { return _carDirection; }
	void SetCarDirection(XMFLOAT3 carDirection) { _carDirection = carDirection; }

	// Get/Set Car Velocity
	XMFLOAT3 GetCarVelocity() const { return _carVelocity; }
	void SetEngineSpeed(XMFLOAT3 carVelocity) { _carVelocity = carVelocity; }

	// Get/Set/Add Engine Speed
	float GetEngineSpeed() const { return _engineSpeed; }
	void SetEngineSpeed(float engineSpeed) { _engineSpeed = engineSpeed;}
	void AddEngineSpeed(float engineSpeed);

	// Get/Set/Add Engine Speed Limit
	float GetEngineSpeedLimit() const { return _engineSpeedLimit; }
	void SetEngineSpeedLimit(float engineSpeedLimit) { _engineSpeedLimit = engineSpeedLimit; }

private:
	float _wheelRadius;
	float _wheelSpeed;
	int	_gearRatio;

	float _engineSpeed;
	float _engineSpeedLimit;

	float _thrust;

	XMFLOAT3 _carVelocity;
	XMFLOAT3 _carDirection;
};

