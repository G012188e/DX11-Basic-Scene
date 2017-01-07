#include "AiPlaneParticleModel.h"


AiPlaneParticleModel::AiPlaneParticleModel(Transform* transform, float mass) : ParticleModel(transform, mass)
{
	_gearRatio = 10;
	_wheelSpeed = 1.0f;
	_engineSpeed = 0.0f;
	_wheelRadius = 10.0f;

	_engineSpeedLimit = 0.3f;

	_carVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

AiPlaneParticleModel::~AiPlaneParticleModel()
{
}

void AiPlaneParticleModel::AddEngineSpeed(float engineSpeed)
{
	_engineSpeed += engineSpeed;
	TruncateSpeed();
}

void AiPlaneParticleModel::CalculateWheelSpeed()
{
	TruncateSpeed();

	_wheelSpeed = _engineSpeed / _gearRatio;
}

void AiPlaneParticleModel::TruncateSpeed()
{
	if (_engineSpeed > _engineSpeedLimit)
	{
		_engineSpeed = _engineSpeedLimit;
	}
}

void AiPlaneParticleModel::CalculateThrust(float t)
{
	_thrust = ((_wheelSpeed * 60) * 2 * XM_PI * _wheelRadius) / t;
}

void AiPlaneParticleModel::CalculateVelocity()
{
	_carVelocity.x = _carDirection.x * _thrust;
	_carVelocity.y = _carDirection.y * _thrust;
	_carVelocity.z = _carDirection.z * _thrust;
}

void AiPlaneParticleModel::Update(float t)
{
	CalculateWheelSpeed();
	CalculateThrust(t);
	CalculateVelocity();

	// Update Particle Model
	ParticleModel::Update(t);
}