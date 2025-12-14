#include "Wall.h"

Wall::Wall()
{
    m_Position = {0.f, 0.f, 0.f};
    Tag = "Wall";
}

void Wall::Start()
{
    m_Rb->SetVelocity({0, 0});
    m_Rb->SetCollisionEnabled(true);
    m_Rb->SetCollisionChannel(ECollisionChannel::WorldStatic);
    m_Rb->SetCollisionResponse(ECollisionChannel::WorldDynamic, ECollisionResponse::Block);
}

void Wall::Update(float deltaTime)
{
    
}

void Wall::Draw()
{
}
