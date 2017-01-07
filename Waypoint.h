#ifndef WAYPOINT_H
#define WAYPOINT_H


#include <vector>

#include <directxmath.h>

using namespace::std;
using namespace::DirectX;

//---------------------------------------------------------------
// A waypoint is a point on the map which information regarding where can be reached from this point.
// Used in pathfinding.

class Texture2D;

class Waypoint
{
	//---------------------------------------------------------------
public:
	Waypoint(int id, XMFLOAT3 startPosition, vector<int> connectingIDs);
	~Waypoint();

	int			GetID()										{return mID;}
	
	vector<int> GetConnectedWaypointIDs()					{return mConnectedWaypointIDs;}
	bool		IsConnectedTo(int waypointIDToCheck);

	XMFLOAT3	GetPosition()								{ return mPosition; }


	//---------------------------------------------------------------
private:
	int				mID;
	XMFLOAT3		mPosition;
	Texture2D*		mTexture;

	vector<int>	    mConnectedWaypointIDs;			
};

//---------------------------------------------------------------

#endif //WAYPOINT_H