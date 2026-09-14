#pragma once

#include "TravelersRecall.h"

class TravelersRecallCommandScript : public CommandScript
{
public:
    TravelersRecallCommandScript();

    Acore::ChatCommands::ChatCommandTable GetCommands() const override;

    static bool SendListForPlayer(Player* player);
    static bool HandleListCommand(ChatHandler* handler, char const* args);
    static bool HandleTeleportCommand(ChatHandler* handler, char const* args);
    static bool HandleLearnCommand(ChatHandler* handler, char const* args);
    static bool HandleLearnAllCommand(ChatHandler* handler, char const* args);
    static bool HandleRemoveCommand(ChatHandler* handler, char const* args);
    static bool HandleRemoveAllCommand(ChatHandler* handler, char const* args);
};
