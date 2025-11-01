#include "PongTerminal.h"
#include "Engine/Engine.h"
#include "Core/Globals.h"
#include "Core/Logger.h"
#include "Input/InputManager.h"
#include "Engine/AssetManager.h"
#include "Engine/TextureAsset.h"
#include "Render/Renderer.h"

void ApplicationInit()
{
    GlobalParameters::g_ViewportWidth = 80;
    GlobalParameters::g_ViewportHeight = 25;    
    GlobalParameters::g_ColorMode = EColorMode::TrueColor;
    GlobalParameters::g_bUseWindowsApiOutput = false;
}

class PongTerminal : public IGame
{
public:

    virtual void Init() override;
    virtual void Start() override {}
    virtual void Tick(float deltaTime) override;
    virtual void TickFrame(float deltaTime) override;
    virtual void End() override {}
    virtual void Shutdown() override {}

private:
    Vector m_Pos = {0, 0, 0};

    TAssetPtr<TextureAsset> m_GTex;
    TAssetPtr<TextureAsset> m_GTex3;
    TAssetPtr<TextureAsset> m_GTex4;
    TAssetPtr<TextureAsset> m_GTex9;
    TAssetPtr<TextureAsset> m_GTex10;
    TAssetPtr<TextureAsset> m_GTexTrnt;
};

void PongTerminal::Init()
{
    m_GTex = AssetManager::Get().LoadAsset<TextureAsset>("/Engine/GTest.thtx");
    m_GTex3 = AssetManager::Get().LoadAsset<TextureAsset>("/Engine/GTest3.thtx");
    m_GTex4 = AssetManager::Get().LoadAsset<TextureAsset>("/Engine/GTest4.thtx");
    m_GTex9 = AssetManager::Get().LoadAsset<TextureAsset>("/Engine/GTest9.thtx");
    m_GTex10 = AssetManager::Get().LoadAsset<TextureAsset>("/Engine/GTest10.thtx");
    m_GTexTrnt = AssetManager::Get().LoadAsset<TextureAsset>("/Engine/GTestTrnt.thtx");
}

void PongTerminal::Tick(float deltaTime)
{
    auto& input = InputManager::Get();

    float speed = 40.f;

    if (input.GetKey(Key::D))
    {
        m_Pos.x += speed * deltaTime;
    }
    if (input.GetKey(Key::A))
    {
        m_Pos.x -= speed * deltaTime;
    }
    if (input.GetKey(Key::W))
    {
        m_Pos.y -= speed * 0.5f * deltaTime;
    }
    if (input.GetKey(Key::S))
    {
        m_Pos.y += speed * 0.5f * deltaTime;
    }


    
    if (m_GTex3.IsValid())
    {
        Renderer::GetConsole2D().DrawQuad(iVector(m_Pos.x, m_Pos.y, 0), m_GTex3->GetData().get());
    }
    
    if (m_GTex4.IsValid())
    {
        Renderer::GetConsole2D().DrawQuad(iVector(m_Pos.x + 15, m_Pos.y, 0), m_GTex4->GetData().get());
    }
    
    if (m_GTex9.IsValid())
    {
        Renderer::GetConsole2D().DrawQuad(iVector(m_Pos.x + 30, m_Pos.y, 0), m_GTex9->GetData().get());
    }
    if (m_GTex10.IsValid())
    {
        Renderer::GetConsole2D().DrawQuad(iVector(m_Pos.x + 45, m_Pos.y, 0), m_GTex10->GetData().get());
    }
    if (m_GTexTrnt.IsValid())
    {
        Renderer::GetConsole2D().DrawQuad(iVector(m_Pos.x + 60, m_Pos.y, 0), m_GTexTrnt->GetData().get());
    }
    


}

void PongTerminal::TickFrame(float deltaTime)
{

}

std::unique_ptr<IGame> CreateGameInstance()
{
    return std::make_unique<PongTerminal>();
}