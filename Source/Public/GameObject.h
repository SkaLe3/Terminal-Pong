#pragma once
#include "Core/Delegate.h"
#include "Core/Math/Math.h"

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void Start() {}
    virtual void Update(float deltaTime) = 0;
    virtual void NetUpdate() {}
    virtual void Draw() = 0;

    Vector& GetPosition() { return m_Position; }
    void SetActive(bool bActive) {m_bActive = bActive;}
    bool IsActive() {return m_bActive;}

public:
    MulticastDelegate<GameObject*, Vector2> OnHit;
    std::string Tag;
protected:
    Vector m_Position;
    bool m_bActive = true;
};