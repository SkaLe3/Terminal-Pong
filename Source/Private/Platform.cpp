#include "Platform.h"
#include "Render/Sprite.h"
#include "Assets/AssetManager.h"
#include "Engine/Engine.h"
#include "Input/InputManager.h"
#include "Core/Logger.h"

#include "PongNetwork.h"

#include <limits>

Platform::Platform(int32 playerIndex, int32 paddleIndex)
{
	m_PrevPosX = std::numeric_limits<float>::max();
	m_PlayerIndex = playerIndex;
	m_PaddleIndex = paddleIndex;
#ifndef DEDICATED_SERVER
	TAssetPtr<SpriteAsset> platformSpt;
	if (paddleIndex == 0)
	{
		platformSpt = AssetManager::Get().LoadAsset<SpriteAsset>("/Game/PadLower.thsprite");
	}
	else if (paddleIndex == 1)
	{
		platformSpt = AssetManager::Get().LoadAsset<SpriteAsset>("/Game/PadLower.thsprite");
	}
	m_PlatformSprite = Sprite::Create(platformSpt, { (platformSpt->GetRect().width) / 2, (platformSpt->GetRect().height) / 2 });
#endif
	m_MoveSpeed = 35.f;
	Tag = "Platform";
}

void Platform::Start()
{
	m_Rb->SetCollider(std::make_shared<BoxCollider>(Vector2{ 11.6, 2 }));
	m_Rb->SetVelocity({ 0, 0 });
	m_Rb->SetCollisionEnabled(true);
	m_Rb->SetCollisionChannel(ECollisionChannel::WorldDynamic);
	m_Rb->SetCollisionResponse(ECollisionChannel::WorldDynamic, ECollisionResponse::Overlap);
	m_Rb->SetCollisionResponse(ECollisionChannel::WorldStatic, ECollisionResponse::Block);

	OnHit.Add(this, &Platform::HandleCollision);

#ifdef DEDICATED_SERVER
	PongNetwork& net = PongNetwork::Get();

	if (m_PlayerIndex == 0)
	{

		net.OnInputPlayer0 = [&](const InputMessage& input)
			{
				m_InputDirection.x = input.Move;
				m_Rb->SetVelocity(m_InputDirection * m_MoveSpeed);
			};
	}
	if (m_PlayerIndex == 1)
	{
		net.OnInputPlayer1 = [&](const InputMessage& input)
			{
				m_InputDirection.x = input.Move;
				m_Rb->SetVelocity(m_InputDirection * m_MoveSpeed);
			};
	}
#else
	PongNetwork& net = PongNetwork::Get();
	if (m_PlayerIndex == 0)
	{

		net.OnPaddle0 = [&](const PaddleStateMessage& state)
			{
				GetPosition().x = state.x;
			};
	}
	if (m_PlayerIndex == 1)
	{

		net.OnPaddle1 = [&](const PaddleStateMessage& state)
			{
				GetPosition().x = state.x;
			};
	}
#endif
}

void Platform::Draw()
{
#ifndef DEDICATED_SERVER
	g_Engine->Get2D().DrawSprite(m_Position, m_PlatformSprite);
#endif
}

void Platform::Update(float deltaTime)
{
#ifndef DEDICATED_SERVER

	if (PongNetwork::Get().GetLocalPlayerIndex() == m_PlayerIndex)
	{
		auto& input = InputManager::Get();

		m_InputDirection = { 0, 0 };

		if (input.GetKey(Key::A))
		{
			m_InputDirection.x += -1;
		}
		if (input.GetKey(Key::D))
		{
			m_InputDirection.x += 1;
		}

		//m_Rb->SetVelocity(m_InputDirection * m_MoveSpeed);

		PongNetwork::Get().SendInput(m_InputDirection.x);
	}

#else
	Vector2 sendPos = m_Rb->GetCollider()->Pos;
	if (m_PrevPosX != sendPos.x)
	{
		PaddleStateMessage msg{};
		msg.Type = PongMessage::PaddleState;
		msg.x = sendPos.x;
		msg.playerIndex = m_PlayerIndex;
		PongNetwork::Get().SendPaddleState(msg);
		m_PrevPosX = sendPos.x;
	}
#endif
}

void Platform::HandleCollision(GameObject* other, Vector2 normal)
{

}
