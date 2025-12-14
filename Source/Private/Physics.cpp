#include "Physics.h"
#include <cmath>

namespace Physics
{
    bool IsColliding(const BoxCollider& a, const BoxCollider& b)
    {
            double dx = std::abs(a.Pos.x - b.Pos.x);
    		double dy = std::abs(a.Pos.y - b.Pos.y);

    		double overlapX = (a.HalfSize.x + b.HalfSize.x);
    		double overlapY = (a.HalfSize.y + b.HalfSize.y);

    		return (dx <= overlapX) && (dy <= overlapY);
    }

    BoxCollider GetBroadPhaseCollider(const BoxCollider& collider)
    {
        Vector2 position = collider.Pos + collider.Velocity * 0.5;
		Vector2 size = collider.HalfSize  + collider.Velocity.GetAbs() * 0.5;

		return BoxCollider{size * 2.f , position, collider.Velocity};
    }
    double SweptAABB(const BoxCollider& collider, const BoxCollider& other, const Vector2& frameMovement, Vector2& normal)
    {
        // Broad-Phase 
		BoxCollider bpc = GetBroadPhaseCollider(collider);
		if (!IsColliding(bpc, other))
			return 1;
		else
		{
			int a = 5;
		}

		Vector2 dEntry;
		Vector2 dExit;


		if (frameMovement.x > 0.0)
		{
			dEntry.x = (other.Pos.x - other.HalfSize.x) - (collider.Pos.x + collider.HalfSize.x);
			dExit.x = (other.Pos.x + other.HalfSize.x) - (collider.Pos.x - collider.HalfSize.x);
		}
		else
		{
			dEntry.x = (other.Pos.x + other.HalfSize.x) - (collider.Pos.x - collider.HalfSize.x);
			dExit.x = (other.Pos.x - other.HalfSize.x) - (collider.Pos.x + collider.HalfSize.x);
		}
		if (frameMovement.y > 0.0)
		{
			dEntry.y = (other.Pos.y - other.HalfSize.y) - (collider.Pos.y + collider.HalfSize.y);
			dExit.y = (other.Pos.y + other.HalfSize.y) - (collider.Pos.y - collider.HalfSize.y);
		}
		else
		{
			dEntry.y = (other.Pos.y + other.HalfSize.y) - (collider.Pos.y - collider.HalfSize.y);
			dExit.y = (other.Pos.y - other.HalfSize.y) - (collider.Pos.y + collider.HalfSize.y);
		}

		Vector2 tEntry;
		Vector2 tExit;

		if (frameMovement.x == 0.0)
		{
			tEntry.x = - std::numeric_limits<double>::infinity();
			tExit.x = std::numeric_limits<double>::infinity();
		}
		else
		{
			tEntry.x = dEntry.x / frameMovement.x;
			tExit.x = dExit.x / frameMovement.x;
		}
		if (frameMovement.y == 0.0)
		{
			tEntry.y = -std::numeric_limits<double>::infinity();
			tExit.y = std::numeric_limits<double>::infinity();
		}
		else
		{
			tEntry.y = dEntry.y / frameMovement.y;
			tExit.y = dExit.y / frameMovement.y;
		}

		double entryTime = std::max(tEntry.x, tEntry.y);
		double exitTime = std::min(tExit.x, tExit.y);

		// no collision if entry after exit
		if (entryTime > exitTime) 
		{
    		normal = {0,0};
    		return 1.0;
		}

		// collision happened outside [0,1] time-window
		if (entryTime < 0.0 || entryTime > 1.0) 
		{
    		normal = {0,0};
    		return 1.0;
		}
		else 
		{
			if (tEntry.x > tEntry.y)
			{
				if (dEntry.x < 0.0)
					normal = { 1, 0 };
				else
					normal = { -1, 0 };
			}
			else
			{
				if (dEntry.y < 0.0)
					normal = { 0, 1 };
				else
					normal = { 0, -1 };
			}
		}
		return entryTime;
    }
}