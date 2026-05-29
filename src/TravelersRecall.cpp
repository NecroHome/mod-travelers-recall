#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "Config.h"
#include "Log.h"
#include "ChatCommand.h"
#include "Tokenize.h"
#include "GameTime.h"
#include <chrono>

#include "DatabaseEnv.h"

using namespace std::chrono;

static void SendAddonMessage(Player* player, std::string const& message)
{
    ChatHandler(player->GetSession()).SendSysMessage(message);
}

class TravelersRecallPlayerScript : public PlayerScript
{

    public: TravelersRecallPlayerScript() : PlayerScript("TravelersRecallPlayerScript") { }

    void OnPlayerUpdateArea(Player* player, uint32 oldArea, uint32 newArea) override
    {
        if (player->isDead()) 
        {
            return;
        }

        if (oldArea == newArea)
        {
            return;
        }

        if (!sConfigMgr->GetOption<bool>("TravelersRecall.Enable", false))
        {
            return;
        }

        QueryResult locationResult = WorldDatabase.Query(
            "SELECT id, name, faction, icon FROM custom_travelers_recall_locations WHERE area_id = {}",
            newArea
        );

        if (!locationResult)
        {
            return;
        }

        Field* locationFields = locationResult->Fetch();

        uint32 locationId = locationFields[0].Get<uint32>();
        std::string locationName = locationFields[1].Get<std::string>();
        uint8 faction = locationFields[2].Get<uint8>();
        std::string icon = locationFields[3].Get<std::string>();

        if (!player->IsGameMaster()) 
        {
            TeamId playerTeam = player->GetTeamId();

            if (faction == 1 && playerTeam != TEAM_ALLIANCE) 
            {
                return;
            }

            if (faction == 2 && playerTeam != TEAM_HORDE) 
            {
                return;
            }
        }

        QueryResult unlockResult = CharacterDatabase.Query(
            "SELECT 1 FROM custom_travelers_recall_unlocks WHERE guid = {} AND location_id = {}",
            player->GetGUID().GetCounter(),
            locationId
        );

        if (unlockResult)
        {
            return;
        }

        CharacterDatabase.Execute(
            "INSERT INTO custom_travelers_recall_unlocks (guid, location_id) VALUES ({}, {})",
            player->GetGUID().GetCounter(),
            locationId
        );

        ChatHandler(player->GetSession()).PSendSysMessage(
            "New location unlocked: {}",
            locationName
        );

        std::string message = Acore::StringFormat(
            "TR_UNLOCK:{}:{}:{}",
            locationId,
            locationName,
            icon
        );

        SendAddonMessage(player, message);

        LOG_INFO(
            "module",
            "Traveler's Recall: player {} unlocked {}",
            player->GetName(),
            locationName
        );
    }

    void OnPlayerDelete(ObjectGuid guid, uint32 account) override
    {
        CharacterDatabase.Execute(
            "DELETE FROM custom_travelers_recall_unlocks "
            "WHERE guid = {}",
            guid.GetCounter()
        );
    }
};

class TravelersRecallCommandScript : public CommandScript
{
    public: TravelersRecallCommandScript() : CommandScript("TravelersRecallCommandScript") { }

    Acore::ChatCommands::ChatCommandTable GetCommands() const override
    {
        static Acore::ChatCommands::ChatCommandTable teleportTable =
        {
            { "list", HandleListCommand, SEC_PLAYER, Acore::ChatCommands::Console::No },
            { "learn", HandleLearnCommand, SEC_GAMEMASTER, Acore::ChatCommands::Console::No },
            { "learn all", HandleLearnAllCommand, SEC_GAMEMASTER, Acore::ChatCommands::Console::No },
            { "teleport", HandleTeleportCommand, SEC_PLAYER, Acore::ChatCommands::Console::No }
        };

        static Acore::ChatCommands::ChatCommandTable commandTable =
        {
            { "tr", teleportTable }
        };

        return commandTable;
    }

    static bool HandleListCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetPlayer();

        QueryResult unlocks = CharacterDatabase.Query(
            "SELECT location_id "
            "FROM custom_travelers_recall_unlocks "
            "WHERE guid = {}",
            player->GetGUID().GetCounter()
        );

        if (!unlocks)
        {
            return true;
        }

        do
        {
            Field* unlockFields = unlocks->Fetch();

            uint32 locationId = unlockFields[0].Get<uint32>();

            QueryResult location = WorldDatabase.Query(
                "SELECT name, icon "
                "FROM custom_travelers_recall_locations "
                "WHERE id = {}",
                locationId
            );

            if (!location)
            {
                continue;
            }

            Field* locationFields = location->Fetch();

            std::string locationName = locationFields[0].Get<std::string>();
            std::string icon = locationFields[1].Get<std::string>();

            std::string message = Acore::StringFormat(
                "TR_LIST:{}:{}:{}",
                locationId,
                locationName,
                icon
            );

            ChatHandler(player->GetSession()).SendSysMessage(message.c_str());

        } while (unlocks->NextRow());

        return true;
    }

    static bool HandleTeleportCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetPlayer();
        uint32 locationId = atoi(args);

        if (!CanPlayerTeleport(player, handler, locationId)) 
        {
            return true;
        }

        QueryResult location = WorldDatabase.Query(
            "SELECT "
            "map_id, "
            "position_x, "
            "position_y, "
            "position_z, "
            "orientation, "
            "name, "
            "cooldown "
            "FROM custom_travelers_recall_locations "
            "WHERE id = {}",
            locationId
        );

        if (!location)
        {
            handler->SendSysMessage("Traveler's Recall: location not found.");
            return false;
        }

        Field* fields = location->Fetch();

        uint32 mapId = fields[0].Get<uint32>();

        float x = fields[1].Get<float>();
        float y = fields[2].Get<float>();
        float z = fields[3].Get<float>();
        float o = fields[4].Get<float>();

        std::string locationName = fields[5].Get<std::string>();
        
        uint32 cooldown = fields[6].Get<uint32>();

        if (cooldown == 0 && sConfigMgr->GetOption<bool>("TravelersRecall.UseDefaultCooldown", false))
        {
            cooldown = sConfigMgr->GetOption<uint32>("TravelersRecall.DefaultCooldown", 0);
        }

        if (cooldown > 0)
        {
            QueryResult cooldownResult = CharacterDatabase.Query(
                "SELECT cooldown_end "
                "FROM custom_travelers_recall_unlocks "
                "WHERE guid = {} AND location_id = {}",
                player->GetGUID().GetCounter(),
                locationId
            );

            if (cooldownResult)
            {
                uint32 cooldownEnd = cooldownResult->Fetch()[0].Get<uint32>();

                uint32 now = duration_cast<Seconds>(GameTime::GetSystemTime().time_since_epoch()).count();

                if (cooldownEnd > now)
                {
                    uint32 remaining = cooldownEnd - now;
                    uint32 remainingMinutes = remaining / 60;
                    uint32 remainingSeconds = remaining % 60;

                    handler->PSendSysMessage(
                        "Traveler's Recall: cooldown remaining for {}: {} min {} sec.",
                        locationName,
                        remainingMinutes,
                        remainingSeconds
                    );

                    return true;
                }
            }
        }

        if (!player->TeleportTo(mapId, x, y, z, o))
        {
            handler->SendSysMessage("Traveler's Recall: teleportation failed.");
            return false;
        }
        else
        {
            if (Group* group = player->GetGroup())
            {
                for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                {
                    Player* partyMember = itr->GetSource();

                    if (!partyMember) 
                    {
                        continue;
                    }

                    if (!partyMember->GetSession())
                    {
                        continue;
                    }

                    if (!partyMember->IsInWorld())
                    {
                        continue;
                    }

                    if (partyMember->GetSession()->IsBot()) 
                    {
                        if(!partyMember->TeleportTo(mapId, x, y, z, o))
                        {
                            handler->SendSysMessage("Traveler's Recall: party bot teleportation failed.");
                        }
                    }
                }
            }
        }


        if (cooldown > 0)
        {
            uint32 cooldownEnd = duration_cast<Seconds>(GameTime::GetSystemTime().time_since_epoch()).count() + cooldown;

            CharacterDatabase.Execute(
                "UPDATE custom_travelers_recall_unlocks "
                "SET cooldown_end = {} "
                "WHERE guid = {} AND location_id = {}",
                cooldownEnd,
                player->GetGUID().GetCounter(),
                locationId
            );
        }

        handler->PSendSysMessage(
            "Traveler's Recall: teleported to {}",
            locationName
        );

        return true;
    }

    static bool HandleLearnCommand(ChatHandler* handler, char const* args) 
    {
        Player* target = handler->getSelectedPlayerOrSelf();

        if (!target) 
        {
            return false;
        }

        if (!args || !*args)
        {
            handler->SendSysMessage("Traveler's Recall: location id required.");
            return true;
        }

        uint32 area_id = atoi(args);

        if (area_id == 0)
        {
            handler->SendSysMessage("Traveler's Recall: invalid area id.");
            return true;
        }

        uint32 faction = target->GetTeamId() == TEAM_ALLIANCE ? 1 : 2;
        
        QueryResult result = WorldDatabase.Query(
            "SELECT id "
            "FROM custom_travelers_recall_locations "
            "WHERE faction IN (0, {}) "
            "AND area_id = {}",
            faction,
            area_id
        );

        if (!result)
        {
            handler->SendSysMessage("Traveler's Recall: location not found.");
            return true;
        }

        uint32 locationId = result->Fetch()[0].Get<uint32>();

        CharacterDatabase.Execute(
            "INSERT IGNORE INTO custom_travelers_recall_unlocks "
            "(guid, location_id, unlocked_at, cooldown_end) "
            "VALUES ({}, {}, NOW(), 0)",
            target->GetGUID().GetCounter(),
            locationId
        );

        handler->SendSysMessage("Traveler's Recall: location unlocked.");
        return true;
    }

    static bool HandleLearnAllCommand(ChatHandler* handler, char const* args) 
    {
        Player* target = handler->getSelectedPlayerOrSelf();

        if (!target)
        {
            return false;
        }

        uint32 faction = target->GetTeamId() == TEAM_ALLIANCE ? 1 : 2;

        QueryResult result = WorldDatabase.Query(
            "SELECT id "
            "FROM custom_travelers_recall_locations "
            "WHERE faction IN (0, {})",
            faction
        );

        if (!result)
            return false;

        uint32 guid = target->GetGUID().GetCounter();

        do
        {
            uint32 locationId = result->Fetch()[0].Get<uint32>();

            CharacterDatabase.Execute(
                "INSERT IGNORE INTO custom_travelers_recall_unlocks "
                "(guid, location_id, unlocked_at, cooldown_end) "
                "VALUES ({}, {}, NOW(), 0)",
                guid,
                locationId
            );

        } while (result->NextRow());

        handler->SendSysMessage("Traveler's Recall: all locations unlocked.");

        return true;
    }

    static bool HasUnlockedLocation(Player* player, uint32 locationId)
    {
        QueryResult result = CharacterDatabase.Query(
            "SELECT 1 FROM custom_travelers_recall_unlocks "
            "WHERE guid = {} "
            "AND location_id = {}",
            player->GetGUID().GetCounter(),
            locationId
        );

        return result != nullptr;
    }

    static bool CanPlayerTeleport(Player* player, ChatHandler* handler, uint32 locationId)
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
};

void Addmod_travelers_recallScripts()
{
    new TravelersRecallPlayerScript();
    new TravelersRecallCommandScript();
}