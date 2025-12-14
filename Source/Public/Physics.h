#pragma once
#include "Core/Core.h"
#include "PhysicsWorld.h"

namespace Physics
{
    bool IsColliding(const BoxCollider& collider, const BoxCollider& other);
    BoxCollider GetBroadPhaseCollider(const BoxCollider& collider);
    double SweptAABB(const BoxCollider& collider, const BoxCollider& other, const Vector2& frameMovement, Vector2& normal);
}