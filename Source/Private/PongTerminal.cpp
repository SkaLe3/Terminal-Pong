#include "PongTerminal.h"
#include "Engine/Engine.h"
#include "Core/Globals.h"
#include "Core/Logger.h"



void ApplicationInit()
{
    GlobalParameters::g_ViewportWidth = 80;
    GlobalParameters::g_ViewportHeight = 25;    
    GlobalParameters::g_ColorMode = EColorMode::Ansi16;
    GlobalParameters::g_bUseWindowsApiOutput = true;
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
    float m_Pos = 0;
};

void PongTerminal::Init()
{

}

void PongTerminal::Tick(float deltaTime)
{

}

void PongTerminal::TickFrame(float deltaTime)
{

}

std::unique_ptr<IGame> CreateGameInstance()
{
    return std::make_unique<PongTerminal>();
}