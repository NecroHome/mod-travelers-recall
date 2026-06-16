----------------------------------------------------------------------------------------

Traveler's Recall Module

----------------------------------------------------------------------------------------
## For AzerothCore Playerbots
The module as first created for the Playerbots branch, it may or may not work with AzeerothCore "Vanilla"

An AzerothCore "Vanilla" module is in the works

## Description

Progressive fast-travel system for AzerothCore.

## Features

- Unlock teleport locations by discovering cities and important places
- Custom teleport window integration
- Global or individual teleport cooldowns
- Faction restrictions
- Combat / Dungeon / Battleground / Dead / Stealth protection
- Custom unlockable locations

## Required Addon

Install the client addon:

```text
https://github.com/NecroHome/TravelersRecall
```

The addon window can be opened using:

- `/tr`
- or the minimap button

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

## Customization

### Individual Cooldowns

Inside:

```text
acore_world.custom_travelers_recall_locations
```

the `cooldown` column can be configured per location.

- `0`
  - uses the global module cooldown configuration
- greater than `0`
  - uses an individual cooldown value (in seconds)

Example:

```text
3600 = 1 hour
7200 = 2 hours
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
(159, 'Brill', 0, 2259.25, 290.43, 34.1137, 2.503233, 2, 'Interface\\Icons\\Spell_Arcane_TeleportUnderCity', 0, 0);
```

### Column Explanation

| Column | Description |
|---|---|
| `area_id` | Area ID used to detect location discovery |
| `name` | Name displayed in the addon |
| `map_id` | Destination map ID |
| `position_x/y/z` | Teleport coordinates |
| `orientation` | Player facing direction after teleport |
| `faction` | Faction restriction |
| `icon` | WoW icon path used in the addon |
| `required_level` | Reserved for future use |
| `cooldown` | Individual cooldown in seconds |

### Faction Values

| Value | Faction |
|---|---|
| `0` | Neutral |
| `1` | Alliance |
| `2` | Horde |

### Retrieving Area Information

Use:

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

- No worldserver restart is required after adding locations
- Players already inside the area must leave and re-enter it before the teleport unlocks
- Icons use standard World of Warcraft client icon paths

## Author

NecroHome

Change Log

### 29/05/2026

* Added new locations.
* Added GM commands:

  * `.tr learn {area_id}`
  * `.tr learn all`

### 20/05/2026

* Added bot party teleport support.
* Fixed location discovery while dead.
* Added Brill, Razor Hill, and Camp Taurajo.
