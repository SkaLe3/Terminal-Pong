#pragma once

#include "Core/Math/Math.h"
#include "GameObject.h"
#include "PhysicsWorld.h"

class Sprite;

class Platform : public GameObject
{
public:
    Platform(int32 playerIndex, int32 paddleIndex);

    virtual void Start() override;
    virtual void Draw() override;
    virtual void Update(float deltaTime) override;

    void SetRb(std::shared_ptr<RigidBody> rb) {m_Rb = rb; m_Rb->SetOwner(this);}
    std::shared_ptr<RigidBody> GetRb() {return m_Rb;}
private:
    void HandleCollision(GameObject* other, Vector2 normal);

private:
    std::shared_ptr<Sprite> m_PlatformSprite;
    std::shared_ptr<RigidBody> m_Rb;
    Vector2 m_InputDirection = { 0, 0 };
    float m_MoveSpeed = 40;
    int32 m_PlayerIndex;
    int32 m_PaddleIndex;

    float m_PrevPosX;
};
