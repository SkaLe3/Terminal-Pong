#pragma once
#include "Core/Math/Math.h"
#include "GameObject.h"
#include "PhysicsWorld.h"

class Wall : public GameObject
{
    public:
    Wall();

    virtual void Start() override;
    virtual void Update(float deltaTime) override;
    virtual void Draw() override;

    void SetRb(std::shared_ptr<RigidBody> rb) {m_Rb = rb; ; m_Rb->SetOwner(this);}
private:
    std::shared_ptr<RigidBody> m_Rb;
};