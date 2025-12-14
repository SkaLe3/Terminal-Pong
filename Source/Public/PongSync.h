#pragma once
#include "Core/Core.h"

enum class PongMessage : uint8
{
    Connect,
    BallState,
    PaddleState,
    Input
};

struct BallStateMessage
{
    PongMessage Type;
    float x;
    float y;
};

struct PaddleStateMessage
{
    PongMessage Type;
    float x;
    int32 playerIndex = -1;
};

struct InputMessage
{
    PongMessage Type;
    float Move; // -1..1
};

struct ConnectMessage
{
    PongMessage Type;
    int32 PlayerIndex;
};