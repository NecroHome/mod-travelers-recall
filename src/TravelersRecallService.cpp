#include "TravelersRecallService.h"

namespace TravelersRecall
{
    bool HasUnlockedLocation(Player* player, uint32 locationId)
    {
        QueryResult result = CharacterDatabase.Query(
            "SELECT 1 FROM custom_travelers_recall_unlocks WHERE guid = {} AND location_id = {}",
            player->GetGUID().GetCounter(),
            locationId);

        return result != nullptr;
    }

    bool CanPlayerTeleport(Player* player, ChatHandler* handler, uint32 locationId)
    {
        if (player->IsGameMaster())
        {
            return true;
        }

        if (!HasUnlockedLocation(player, locationId))
        {
            handler->SendSysMessage("Traveler's Recall: location not unlocked.");
            return false;
        }

        bool allowInFlight = sConfigMgr->GetOption<bool>("TravelersRecall.CanTeleport.InFlight", false);
        if (player->IsInFlight() && !allowInFlight)
        {
            handler->SendSysMessage("Traveler's Recall: cannot teleport while flying.");
            return false;
        }

        bool allowWhileStealthed = sConfigMgr->GetOption<bool>("TravelersRecall.CanTeleport.WhileStealthed", false);
        if (player->HasStealthAura() && !allowWhileStealthed)
        {
            handler->SendSysMessage("Traveler's Recall: cannot teleport while stealthed.");
            return false;
        }

        bool allowInCombat = sConfigMgr->GetOption<bool>("TravelersRecall.CanTeleport.InCombat", false);
        if (player->IsInCombat() && !allowInCombat)
        {
            handler->SendSysMessage("Traveler's Recall: cannot teleport while in combat.");
            return false;
        }

        bool allowWhileDead = sConfigMgr->GetOption<bool>("TravelersRecall.CanTeleport.WhileDead", false);
        if (player->isDead() && !allowWhileDead)
        {
            handler->SendSysMessage("Traveler's Recall: cannot teleport while dead.");
            return false;
        }

        bool allowInBattleground = sConfigMgr->GetOption<bool>("TravelersRecall.CanTeleport.InBattleground", false);
        if (player->InBattleground() && !allowInBattleground)
        {
            handler->SendSysMessage("Traveler's Recall: cannot teleport from battlegrounds.");
            return false;
        }

        bool allowInDungeonOrRaid = sConfigMgr->GetOption<bool>("TravelersRecall.CanTeleport.InDungeonOrRaid", false);
        if (player->GetMap()->IsDungeon() && !allowInDungeonOrRaid)
        {
            handler->SendSysMessage("Traveler's Recall: cannot teleport from dungeons or raids.");
            return false;
        }

        return true;
    }
}
