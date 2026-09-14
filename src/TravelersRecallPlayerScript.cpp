#include "TravelersRecallPlayerScript.h"

TravelersRecallPlayerScript::TravelersRecallPlayerScript()
    : PlayerScript("TravelersRecallPlayerScript")
{
}

void TravelersRecallPlayerScript::OnPlayerUpdateArea(Player* player, uint32 oldArea, uint32 newArea)
{
    if (!sConfigMgr->GetOption<bool>("TravelersRecall.Enable", false))
    {
        return;
    }

    if (player->isDead())
    {
        return;
    }

    if (oldArea == newArea)
    {
        return;
    }

    QueryResult locationResult = WorldDatabase.Query(
        "SELECT id, name, faction, icon, required_level FROM custom_travelers_recall_locations WHERE area_id = {}",
        newArea);

    if (!locationResult)
    {
        return;
    }

    Field* locationFields = locationResult->Fetch();

    uint32 locationId = locationFields[0].Get<uint32>();
    std::string locationName = locationFields[1].Get<std::string>();
    uint8 faction = locationFields[2].Get<uint8>();
    std::string icon = locationFields[3].Get<std::string>();
    uint32 requiredLevel = locationFields[4].Get<uint32>();

    uint32 useRequiredLevel = sConfigMgr->GetOption<uint32>("TravelersRecall.UseRequiredLevel", 0);

    if (useRequiredLevel == 1)
    {
        if (player->GetLevel() < requiredLevel)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("You need to be at least Level {} to unlock {}", requiredLevel, locationName);
            return;
        }
    }

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
        locationId);

    if (unlockResult)
    {
        return;
    }

    CharacterDatabase.Execute(
        "INSERT INTO custom_travelers_recall_unlocks (guid, location_id) VALUES ({}, {})",
        player->GetGUID().GetCounter(),
        locationId);

    ChatHandler(player->GetSession()).PSendSysMessage("New location unlocked: {}", locationName);

    TravelersRecall::SendUnlockMessage(player, locationId, locationName, icon);

    LOG_INFO(
        "module",
        "Traveler's Recall: player {} unlocked {}",
        player->GetName(),
        locationName);
}

void TravelersRecallPlayerScript::OnPlayerDelete(ObjectGuid guid, uint32 account)
{
    CharacterDatabase.Execute(
        "DELETE FROM custom_travelers_recall_unlocks WHERE guid = {}",
        guid.GetCounter());
}

void TravelersRecallPlayerScript::OnPlayerCreated(ObjectGuid guid, uint32 account) 
{
    
}
