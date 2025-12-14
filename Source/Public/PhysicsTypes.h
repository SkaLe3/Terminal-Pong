#pragma once
#include "Core/Core.h"

enum class ECollisionChannel : uint8
{
    WorldStatic, WorldDynamic
};

enum class ECollisionResponse : uint8
{
    Ignore, Overlap, Block
};