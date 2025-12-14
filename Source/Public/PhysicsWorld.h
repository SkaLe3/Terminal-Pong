#pragma once

#include "Core/Math/Math.h"
#include "GameObject.h"
#include "PhysicsTypes.h"

#include <memory>
#include <vector>
#include <unordered_map>

class BoxCollider
{
    public:
    BoxCollider(const Vector2& extents)
    : HalfSize(extents.x * 0.5, extents.y * 0.5)
    {
    }

    BoxCollider(const Vector2& extents, const Vector2& position)
    : HalfSize(extents.x * 0.5, extents.y * 0.5)
    , Pos(position)
    {
    }

    BoxCollider(const Vector2& extents, const Vector2& position, const Vector2& velocity)
    : HalfSize(extents.x * 0.5, extents.y * 0.5)
    , Pos(position)
    , Velocity(velocity)
    {
    }

public:
    Vector2 HalfSize;
    Vector2 Pos;
    Vector2 Velocity = {0, 0};
};


class RigidBody
{
public:
    ECollisionChannel GetCollisionChannel() {return m_CollisionChannel; }
    void SetCollisionChannel(ECollisionChannel channel) {m_CollisionChannel = channel; }
    bool IsCollisionEnabled() {return m_bCollisionEnabled;}
    void SetCollisionEnabled(bool enabled) {m_bCollisionEnabled = enabled;}
    ECollisionResponse GetCollisionResponse(ECollisionChannel channel);
    void SetCollisionResponse(ECollisionChannel channel, ECollisionResponse response) {m_CollisionResponse[channel] = response;}
    std::shared_ptr<BoxCollider> GetCollider();
    GameObject* GetOwner() {return m_Owner;}
    void SetOwner(GameObject* newOwner) {m_Owner = newOwner;}

    void SetCollider(std::shared_ptr<BoxCollider> bc) {m_Collider = bc;}
    void SetVelocity(const Vector2& velocity);

private:
    GameObject* m_Owner;
    std::shared_ptr<BoxCollider> m_Collider;
    bool m_bCollisionEnabled = false;
    ECollisionChannel m_CollisionChannel = ECollisionChannel::WorldStatic;
    std::unordered_map<ECollisionChannel, ECollisionResponse> m_CollisionResponse = 
    {{ECollisionChannel::WorldStatic, ECollisionResponse::Ignore},
    {ECollisionChannel::WorldDynamic, ECollisionResponse::Ignore}};
};


class PhysicsWorld
{
public:
    void Update(float deltaTime);
    std::shared_ptr<RigidBody> CreateRigidBody();
private:
    std::vector<std::shared_ptr<RigidBody>> m_Bodies;
};