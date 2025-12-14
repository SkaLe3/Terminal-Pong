#include "PongTerminal.h"
#include "Engine/Engine.h"
#include "Core/Globals.h"
#include "Core/Logger.h"
#include "Input/InputManager.h"
#include "Assets/AssetManager.h"
#include "Assets/TextureAsset.h"
#include "Assets/SpriteAsset.h"
#include "Render/Renderer.h"


#include "Platform.h"
#include "Ball.h"
#include "Wall.h"
#include "PhysicsWorld.h"
#include "Networking/NetworkingCore.h"
#include "PongNetwork.h"

#include <iostream>

void ApplicationInit()
{
    GlobalParameters::g_ViewportWidth = 61;
    GlobalParameters::g_ViewportHeight = 29;    
    GlobalParameters::g_ColorMode = EColorMode::TrueColor;
    GlobalParameters::g_bUseWindowsApiOutput = false;
}

class PongTerminal : public IGame
{
public:

    virtual void Init() override;
    virtual void Start() override;
    virtual void Tick(float deltaTime) override;
    virtual void TickFrame(float deltaTime) override;
    virtual void End() override {}
    virtual void Shutdown() override {}

private:
    PhysicsWorld m_PhysicsWorld;
    std::vector<std::shared_ptr<GameObject>> m_GameObjects;
    std::shared_ptr<Platform> m_Player;
};

void PongTerminal::Init()
{
    #ifndef DEDICATED_SERVER
    Renderer::Get().SetBgClearColor({0, 0, 0});
    Renderer::Get().SetUseDefaultBgColor(false);
    g_Engine->SetCamera2D(Camera2D{ {0, 0} });
    #endif

    PongNetwork& net = PongNetwork::Get();
    int32 otherPlayerIndex = -1;
    int32 thisPlayerIndex = -1;

    #ifdef DEDICATED_SERVER
    net.StartServer(54000);
    thisPlayerIndex = 0;
    otherPlayerIndex = 1;
    #else
    

    bool bConnected = net.ConnectClient("127.0.0.1", 54000);
    if (!bConnected)
    {
        return;
    }
    while (!net.HasLocalPlayerIndex())
    {
        // Add timeout that indicates unreachable server
        PongNetwork::Get().Poll();
    }
    thisPlayerIndex = net.GetLocalPlayerIndex();
    otherPlayerIndex = net.GetLocalPlayerIndex() == 0 ? 1 : 0;
    #endif

    std::shared_ptr<RigidBody> platformRb1 = m_PhysicsWorld.CreateRigidBody();
    std::shared_ptr<RigidBody> platformRb2 = m_PhysicsWorld.CreateRigidBody();
    std::shared_ptr<RigidBody> ballRb = m_PhysicsWorld.CreateRigidBody();
    std::shared_ptr<Platform> paddle1 = std::make_shared<Platform>(thisPlayerIndex, 0);
    std::shared_ptr<Platform> paddle2 = std::make_shared<Platform>(otherPlayerIndex, 1);
    std::shared_ptr<Ball> ballObject = std::make_shared<Ball>();
    paddle1->SetRb(platformRb1);
    paddle2->SetRb(platformRb2);
    ballObject->SetRb(ballRb);
    m_GameObjects.push_back(paddle1);
    m_GameObjects.push_back(paddle2);
    m_GameObjects.push_back(ballObject);

    paddle1->GetPosition() = Vector{0, 13.5, 1};
    paddle2->GetPosition() = Vector{0, -13.5, 1};

    std::shared_ptr<RigidBody> leftWallRb = m_PhysicsWorld.CreateRigidBody();
    std::shared_ptr<RigidBody> rightWallRb = m_PhysicsWorld.CreateRigidBody();
    std::shared_ptr<RigidBody> floorRb = m_PhysicsWorld.CreateRigidBody();
    std::shared_ptr<RigidBody> ceilingRb = m_PhysicsWorld.CreateRigidBody();

    leftWallRb->SetCollider(std::make_shared<BoxCollider>(Vector2{2, 30}));
    rightWallRb->SetCollider(std::make_shared<BoxCollider>(Vector2{2, 30}));
    floorRb->SetCollider(std::make_shared<BoxCollider>(Vector2{70, 2}));
    ceilingRb->SetCollider(std::make_shared<BoxCollider>(Vector2{70, 2}));

    std::shared_ptr<Wall> leftWall = std::make_shared<Wall>();
    std::shared_ptr<Wall> rightWall = std::make_shared<Wall>();
    std::shared_ptr<Wall> floor = std::make_shared<Wall>();
    std::shared_ptr<Wall> ceiling = std::make_shared<Wall>();

    leftWall->SetRb(leftWallRb);
    rightWall->SetRb(rightWallRb);
    floor->SetRb(floorRb);
    ceiling->SetRb(ceilingRb);

    leftWall->GetPosition() = Vector{-32, 0, 0};
    rightWall->GetPosition() = Vector{32, 0, 0};
    floor->GetPosition() = Vector{0, 15, 0};
    ceiling->GetPosition() = Vector{0, -16, 0};

    m_GameObjects.push_back(leftWall);
    m_GameObjects.push_back(rightWall);
    m_GameObjects.push_back(floor);
    m_GameObjects.push_back(ceiling);
}

void PongTerminal::Start()
{
    for (std::shared_ptr<GameObject> obj : m_GameObjects)
    {
        obj->Start();
    }
}

void PongTerminal::Tick(float deltaTime)
{
    // Poll received messages from the server
    PongNetwork::Get().Poll();

    for (std::shared_ptr<GameObject> obj : m_GameObjects)
    {
        obj->Update(deltaTime);
    }
#ifdef DEDICATED_SERVER
    m_PhysicsWorld.Update(deltaTime);
#endif
    for (std::shared_ptr<GameObject> obj : m_GameObjects)
    {
        obj->Draw();
    }
}

void PongTerminal::TickFrame(float deltaTime)
{

}

std::unique_ptr<IGame> CreateGameInstance()
{
    return std::make_unique<PongTerminal>();
}