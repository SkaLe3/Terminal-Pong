#pragma once
#include "Core/Math/Math.h"
#include "GameObject.h"
#include "PhysicsWorld.h"

class Sprite;

class Ball : public GameObject
{
public:
    Ball();

    virtual void Start() override;
    virtual void Update(float deltaTime) override;
    virtual void NetUpdate() override;
    virtual void Draw() override;

    void SetRb(std::shared_ptr<RigidBody> rb) {m_Rb = rb; ; m_Rb->SetOwner(this);}

private:
    void HandleCollision(GameObject* other, Vector2 normal);
    void UpdateVelocity(float deltaTime);
private:
#ifndef DEDICATED_SERVER
    std::shared_ptr<Sprite> m_BallSprite;
#endif
    std::shared_ptr<RigidBody> m_Rb;

    Vector2 m_MoveDirection;
    float m_Speed;
};
