#include "Ball.h"
#include "Render/Sprite.h"
#include "Engine/Engine.h"
#include "PhysicsWorld.h"
#include "Core/Logger.h"
#include "Assets/AssetManager.h"


#include "Platform.h"
#include "PongNetwork.h"

Ball::Ball()
{
    m_Position = {0.f, 0.f, 2.f};
    m_Speed = 40.f;
#ifndef DEDICATED_SERVER
    m_BallSprite = Sprite::Create(AssetManager::Get().LoadAsset<SpriteAsset>("/Game/Ball.thsprite"));
#endif
    Tag = "Ball";
}

void Ball::Start()
{
    m_Rb->SetCollider(std::make_shared<BoxCollider>(Vector2{1, 1}));
    m_Rb->SetVelocity(Vector2{0.f, -1.f} * m_Speed); // Move in the direction of the player who lost point / or in the second player if the first serve
    m_Rb->SetCollisionEnabled(true);
    m_Rb->SetCollisionChannel(ECollisionChannel::WorldDynamic);
    m_Rb->SetCollisionResponse(ECollisionChannel::WorldDynamic, ECollisionResponse::Block);
    m_Rb->SetCollisionResponse(ECollisionChannel::WorldStatic, ECollisionResponse::Block);

    OnHit.Add(this, &Ball::HandleCollision);

    PongNetwork& net = PongNetwork::Get();
    net.OnBall = [&](const BallStateMessage& msg)
    {
        float recvX = msg.x;
        float recvY = msg.y;
        int32 localPlayer = PongNetwork::Get().GetLocalPlayerIndex();
        if (localPlayer != 0)
        {
            recvY = -recvY;
        }
        GetPosition().x = recvX;
        GetPosition().y = recvY;
    };
}

void Ball::Update(float deltaTime)
{
#ifdef DEDICATED_SERVER
	BallStateMessage msg{};
    msg.Type = PongMessage::BallState;
    msg.x = GetPosition().x;
    msg.y = GetPosition().y;
    PongNetwork::Get().SendBallState(msg);
#endif
}

void Ball::NetUpdate()
{    
    //BallStateMessage msg{};
    //msg.Type = PongMessage::BallState;
    //msg.x = GetPosition().x;
    //msg.y = GetPosition().y;
    //msg.velx = m_Rb->GetCollider()->Velocity.x;
   // msg.vely = m_Rb->GetCollider()->Velocity.y;


}

void Ball::Draw()
{
    #ifndef DEDICATED_SERVER
    if (IsActive())
    {
        g_Engine->Get2D().DrawSprite(m_Position, m_BallSprite);
    }
    #endif
}

void Ball::HandleCollision(GameObject *other, Vector2 normal)
{
    Vector2 velocityReflected = m_Rb->GetCollider()->Velocity.Reflect(normal);
    if (other->Tag == "Platform" && normal.x == 0)
    {
        Platform* platform = static_cast<Platform*>(other);
        std::shared_ptr<RigidBody> rb = platform->GetRb();
        float halfLength = rb->GetCollider()->HalfSize.x;
        float platformPos = rb->GetCollider()->Pos.x;
        float ballPos = m_Rb->GetCollider()->Pos.x;
        float impactRatio = (ballPos - platformPos) / halfLength;
        
        float bounceAngle = -impactRatio * 1.047f; // 60*
		if (normal.y < 0) // top paddle
			bounceAngle = -bounceAngle; // mirror angle
        float speed = m_Speed + std::abs(impactRatio) * 15.f; 
 
        float cs = std::cos(bounceAngle);
        float sn = std::sin(bounceAngle);

        velocityReflected.x = 0;
        velocityReflected = Vector2{
            velocityReflected.x * cs - velocityReflected.y * sn, 
            velocityReflected.x * sn + velocityReflected.y * cs};

        if (std::abs(velocityReflected.y) < 0.1f)
        {
            velocityReflected.y = (velocityReflected.y < 0 ? -0.1f : 0.1f);
        }
        velocityReflected = velocityReflected.GetNormal() * speed;
 
    }
    if (other->Tag == "Wall")
    {
    }
    
    m_Rb->SetVelocity(velocityReflected);
}

void Ball::UpdateVelocity(float deltaTime)
{
    //m_Rb->SetVelocity(m_MoveDirection * m_MoveSpeed * deltaTime * Vector2(1.f, 0.5f));
}
