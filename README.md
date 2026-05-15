----------------------------------------------------------------------------------------

Traveler's Recall Module

----------------------------------------------------------------------------------------

## Description

Progressive fast-travel system for AzerothCore.

## Features

- Unlock teleport locations by discovering cities and important places
- Custom teleport window integration
- Global or individual teleport cooldowns
- Faction restrictions
- Combat / Dungeon / Battleground / Dead / Stealth protection
- Custom unlockable locations
- GM management commands

## This module need an Addon
 - Clone this addon to your client addon folder
 - `https://github.com/NecroHome/TravelersRecall`

## Installation

- Clone the repository into your `modules` folder
- Rebuild AzerothCore
- Run the SQL scripts from:
  - `/data/world`
  - `/data/characters`
- Newer AzerothCore versions execute SQL updates automatically during worldserver startup
- Copy:
  - `mod-travelers-recall.conf.dist`
  - to your `conf` folder
- Rename it to:
  - `mod-travelers-recall.conf`
- Newer AzerothCore versions may copy this automatically
- Install the addon on the client
- The addon window can be opened using:
  - `/tr`
  - or the minimap button

## Customization

### Individual Cooldowns

Inside:

```text
acore_world.custom_travelers_recall_locations
```

the `cooldown` column can be configured per location.

If the value is:

- `0`
  - the module will use the global config cooldown (if enabled)
- otherwise
  - the location will use its own cooldown value

Example:

```text
Stormwind = 7200 (2 hours)
```

instead of the default:

```text
3600 (1 hour)
```

### Adding Custom Locations

Custom teleport locations can be added directly into:

```text
acore_world.custom_travelers_recall_locations
```

Example:

```sql
INSERT IGNORE INTO acore_world.custom_travelers_recall_locations
(area_id, name, map_id, position_x, position_y, position_z, orientation, faction, icon, required_level, cooldown)
VALUES
(159, 'Undercity', 0, 2259.25, 290.43, 34.1137, 2.503233, 2, 'Interface\\Icons\\Spell_Arcane_TeleportUnderCity', 0, 0);
```

### Column Explanation

| Column | Description |
|---|---|
| `area_id` | Area ID used to detect when the player discovers the location |
| `name` | Name displayed in the addon window |
| `map_id` | Destination map ID |
| `position_x` | Teleport X coordinate |
| `position_y` | Teleport Y coordinate |
| `position_z` | Teleport Z coordinate |
| `orientation` | Player facing direction after teleport |
| `faction` | Faction restriction |
| `icon` | WoW icon path displayed in the addon |
| `required_level` | Not used (yet) |
| `cooldown` | Individual cooldown in seconds |

### Faction Values

| Value | Faction |
|---|---|
| `0` | Neutral |
| `1` | Alliance |
| `2` | Horde |

### Cooldown Behavior

If `cooldown` is:

- `0`
  - the global module cooldown configuration will be used
- greater than `0`
  - the location will use its own cooldown value
- the cooldown value is in `seconds`

Example:

```text
3600 = 1 hour
7200 = 2 hours
```

### How To Retrieve Area Information

Use the GM command:

```text
.gps
```

while standing in the desired location.

This provides:

- Area ID
- Map ID
- Coordinates
- Orientation

### Notes

- No worldserver restart is required after inserting new locations
- Players already inside the area must leave and re-enter it before the teleport unlocks
- Icons use the standard World of Warcraft client icon paths

## Author

NecroHome