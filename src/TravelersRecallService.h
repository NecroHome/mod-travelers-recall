#pragma once

#include "TravelersRecall.h"

namespace TravelersRecall
{
    bool HasUnlockedLocation(Player* player, uint32 locationId);
    bool CanPlayerTeleport(Player* player, ChatHandler* handler, uint32 locationId);
}
