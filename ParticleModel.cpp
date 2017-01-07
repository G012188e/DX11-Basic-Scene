#include "ParticleModel.h"

const float ParticleModel::_gravity = -9.81f;

ParticleModel::ParticleModel(Transform* transform, float mass)
{
	_transform = transform;
	_acceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_netForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_slidingForce = XMFLOAT3(0.0f, 0.0f, 0.0f);

	_bBoxdimention = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_origin = XMFLOAT3(0.0f, 0.0f, 0.0f);

	_dragFactor = 1.0f;
	_forceMag = 0.0f;
	_mass = mass;

	_slidingOn = false;

	_useConstAcc = true;
	_laminar = true;

	_collisionRadius = 1.5f;

	_theta = 90.0f;
}

ParticleModel::~ParticleModel()
{

}

// ----------------------- Movement ------------------------- //

void ParticleModel::Move(float amountX, float amountY, float amountZ)
{
	XMFLOAT3 position = _transform->GetPosition();

	position.x += amountX;
	position.y += amountY;
	position.z += amountZ;

	_transform->SetPrevPosition(_transform->GetPosition());

	XMFLOAT3 PREV = _transform->GetPrevPosition();

	_transform->SetPosition(position);
}

void ParticleModel::MoveConstAcc(float t)
{
	t /= 1000.0f;

	XMFLOAT3 position = _transform->GetPosition();

	_constAccel.x = _velocity.x * t + 0.5f * _acceleration.x * t * t;
	_constAccel.y = _velocity.y * t + 0.5f * _acceleration.y * t * t;
	_constAccel.z = _velocity.z * t + 0.5f * _acceleration.z * t * t;

	// Update object position
	position.x += _constAccel.x;
	position.y += _constAccel.y;
	position.z += _constAccel.z;

	// Set velocity
	_velocity.x += (_acceleration.x * 1.4f) * t;
	_velocity.y += (_acceleration.y * 1.4f) * t;
	_velocity.z += (_acceleration.z * 1.4f) * t;

	_transform->SetPosition(position);
}

void ParticleModel::MoveConstVel(float t)
{
	XMFLOAT3 prevPosition = _transform->GetPosition();

	XMFLOAT3 position;
	position.x = prevPosition.x + _velocity.x * t;
	position.y = prevPosition.y + _velocity.y * t;
	position.z = prevPosition.z + _velocity.z * t;

	_transform->SetPosition(position);
}

// -------------------- Sliding -------------------- //

void ParticleModel::SlidingForce(float theta, float frCoef)
{
	_forceMag = _mass * _gravity * (sin(theta) - frCoef * cos(theta));

	_slidingForce.x = _forceMag * cos(theta);
	_slidingForce.y = _forceMag * sin(theta);
	_slidingForce.z = _forceMag * cos(theta);

	if (_slidingForce.y > 0.0f)
	{
		_slidingForce.x = 0.0f;
		_slidingForce.y = 0.0f;
	}
}

void ParticleModel::SetLiftForce(float lift)
{
	_liftForce = lift;
}

// ------------------- Drag Force ------------------ //

void ParticleModel::DragForce()
{
	if (_laminar)
	{
		// Calculate drag force for laminar flow
		DragLamFlow();
	}
	else
	{
		// Calculate drag force for turbulent flow
		TurbulantFlow();
	}
}

void ParticleModel::DragLamFlow()
{
	_dragForce.x = -_dragFactor * _velocity.x;
	_dragForce.y = -_dragFactor * _velocity.y;
	_dragForce.z = -_dragFactor * _velocity.z;
}

void ParticleModel::MotionInFluid(float t)
{
	DragForce();
	MoveConstAcc(t);
}

void ParticleModel::TurbulantFlow()
{
	float velMag;
	// Calculate magnitude of velocity
	velMag = sqrt((_velocity.x * _velocity.x) + (_velocity.y * _velocity.y) + (_velocity.y * _velocity.y));
	
	// Calculate unit vector of velocity
	XMFLOAT3 unitVel;
	unitVel.x = (_velocity.x / velMag);
	unitVel.y = (_velocity.y / velMag);
	unitVel.z = (_velocity.z / velMag);

	// Calculate magnitude of drag force
	float dragMag;
	dragMag = _dragFactor * velMag * velMag;

	// Calculate of x- and y-components of drag force
	_dragForce.x = -dragMag * unitVel.x;
	_dragForce.y = -dragMag * unitVel.y;
}

// ----------------- Collision Detection -------------- //

void ParticleModel::BaseCollisionCheck(XMFLOAT3 floorPos)
{
	XMFLOAT3 objectPos = _transform->GetPosition();

	XMFLOAT3 difference;
	difference.y = objectPos.y - floorPos.y;

	float differenceMag = sqrt(difference.y * difference.y);
//	float velocityMag = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y) + (velocity.z * velocity.z));

	if (differenceMag <= _collisionRadius)
	{
		_velocity.y = -_velocity.y / _mass;
		_slidingOn = true;
	}
	else
	{
		_slidingOn = false;
	}
}

bool ParticleModel::BBoxCollision(ParticleModel* opponent)
{
	XMFLOAT3 obj2Pos = this->GetOrigin();
	XMFLOAT3 obj1Pos = opponent->GetOrigin();

	XMFLOAT3 obj2Box = this->GetBBoxDimention();
	XMFLOAT3 obj1Box = opponent->GetBBoxDimention();

	if (obj1Pos.x > obj2Pos.x + obj1Box.x)
		return false;
	else if (obj1Pos.x + obj1Box.x < obj2Pos.x)
		return false;
	else if (obj1Pos.y > obj2Pos.y + obj1Box.y)
		return false;
	else if (obj1Pos.y + obj1Box.y < obj2Pos.y)
		return false;
	else if (obj1Pos.z > obj2Pos.z + obj1Box.z)
		return false;
	else if (obj1Pos.z + obj1Box.z < obj2Pos.z)
		return false;

	ResolveCollision(opponent);
	return true;
}

bool ParticleModel::CollisionCheck(XMFLOAT3 object2Pos, float object2Radius)
{
	XMFLOAT3 object1Pos = _transform->GetPosition();

	XMFLOAT3 difference;
	difference.x = object1Pos.x - object2Pos.x;
	difference.y = object1Pos.y - object2Pos.y;
	difference.z = object1Pos.z - object2Pos.z;

	float differenceMag = sqrt((difference.x * difference.x) + (difference.y * difference.y) + (difference.z * difference.z));

	float radiusDistance = _collisionRadius + object2Radius;

	if (differenceMag <= radiusDistance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ParticleModel::ResolveCollision(ParticleModel* object2)
{
	//XMFLOAT3 obj1Pos = XMFLOAT3(this->GetOrigin().x + (this->GetBBoxDimention().x / 2), this->GetOrigin().y + (this->GetBBoxDimention().y / 2), this->GetOrigin().z + (this->GetBBoxDimention().z)); //Plane
	//XMFLOAT3 obj2Pos = object2->GetOrigin(); // Tower

	//XMFLOAT3 obj1Box = this->GetBBoxDimention(); // Plane
	//XMFLOAT3 obj2Box = object2->GetBBoxDimention(); // Tower

	//XMFLOAT3 left = XMFLOAT3(obj2Pos.x, obj2Pos.y, obj2Pos.z + (obj2Box.z / 2));
	//XMFLOAT3 right = XMFLOAT3(obj2Pos.x + obj2Box.x, obj2Pos.y, obj2Pos.z + (obj2Box.z / 2));

	//XMFLOAT3 front = XMFLOAT3(obj2Pos.x + (obj2Box.x / 2), obj2Pos.y, obj2Pos.z);
	//XMFLOAT3 back = XMFLOAT3(obj2Pos.x + (obj2Box.x / 2), obj2Pos.y, obj2Pos.z + obj2Box.z);

	//float leftDistance = sqrt((left.x - obj1Pos.x) + (left.y - obj1Pos.y) + (left.z - obj1Pos.z));
	//float rightDistance = sqrt((right.x - obj1Pos.x) + (right.y - obj1Pos.y) + (right.z - obj1Pos.z));
	//float frontDistance = sqrt((front.x - obj1Pos.x) + (front.y - obj1Pos.y) + (front.z - obj1Pos.z));
	//float backDistance = sqrt((back.x - obj1Pos.x) + (back.y - obj1Pos.y) + (back.z - obj1Pos.z));
	//
	//float closest = leftDistance;

	//if (closest > leftDistance) // Plane Left
	//{
	//	closest = leftDistance;
	//}

	//if (closest > rightDistance) //Plane right
	//{
	//	closest = rightDistance;
	//}
	//			
	//if (closest > frontDistance) // Tower Foward / Plane Back
	//{
	//	closest = frontDistance;
	//}
	//	
	//else if (closest > backDistance) // Tower Back / Plane Forward
	//{
	//	closest = frontDistance;
	//}
	//	

	//// do velocity change
	//if (closest == leftDistance) // Plane Left
	//{
	//	_velocity.x *= -2;
	//}

	//if (closest == rightDistance) //Plane right
	//{
	//	_velocity.x *= -2;
	//}

	//if (closest == frontDistance) // Plane front
	//{
	//	_velocity.z *= -2;
	//}

	//else if (closest == backDistance) // Plane back
	//{
	//	_velocity.z *= -2;
	//}

	////_velocity.x = (-_velocity.x * 2.0f) / _mass;
	////_velocity.y = (-_velocity.y * 2.0f) / _mass;
	////_velocity.z = (-_velocity.z * 2.0f) / _mass;

	_velocity.x *= -1.5;
	_velocity.y *= -1.5;
	_velocity.z *= -1.5;

}

// --------------------- Update ----------------------- //

void ParticleModel::UpdateAccel()
{
	_acceleration.x = _netForce.x / _mass;
	_acceleration.y = _netForce.y / _mass;
	_acceleration.z = _netForce.z / _mass;
}

void ParticleModel::UpdateNetForce()
{
	_netForce.x = _dragForce.x + _slidingForce.x;
	_netForce.y = _dragForce.y + _slidingForce.y + _liftForce;
	_netForce.z = _dragForce.z + _slidingForce.z;
}

void ParticleModel::UpdateState()
{
	UpdateNetForce();
	UpdateAccel();
}

void ParticleModel::Update(float t)
{

	if (_useConstAcc)
	{
		if (_slidingOn)
		{
			SlidingForce(XMConvertToRadians(_theta), 0.2f);
		}
		else if(!_slidingOn)
		{
			_slidingForce.x = 0.0f;
			_slidingForce.y = _mass * _gravity;
			_slidingForce.z = 0.0f;
		}
		
		DragForce();
		//LiftForce();
		UpdateState();
		MoveConstAcc(t);
	}
	else
	{
		MoveConstVel(t);
	}
}