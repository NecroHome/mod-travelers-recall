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

### Custom Locations

You can add custom unlockable locations.

Use:

```text
.gps
```

to retrieve the required area information.

After inserting the location into the database:

- no worldserver restart is required
- players already inside that area must leave and re-enter it before the teleport is unlocked

## Author

NecroHome