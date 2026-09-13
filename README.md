# Traveler's Recall Module

Progressive fast-travel system for AzerothCore.

---

## For AzerothCore Playerbots

This module was originally created for the **AzerothCore Playerbots** branch.

It may or may not work with standard AzerothCore ("Vanilla").

If you are using the standard AzerothCore branch, use the dedicated version instead:

[**Mod Traveler's Recall - Vanilla**](https://github.com/NecroHome/mod-travelers-recall/tree/main-vanilla)

---

## Description

Traveler's Recall is a progressive teleport system for AzerothCore.

Players unlock teleport locations by discovering cities and important places throughout the world. Once a location has been unlocked, it becomes available through the Traveler's Recall client addon.

The module handles the teleport logic, unlock progression, cooldowns, faction restrictions, and player-state validation.

---

## Features

* Unlock teleport locations by discovering cities and important places
* Custom teleport window integration
* Global or individual teleport cooldowns
* Faction restrictions
* Player states protections
* Custom unlockable locations
* Bot party teleport support

---

## Required Addon

This module requires the separate client addon:

[**TravelersRecall — Client Addon**](https://github.com/NecroHome/TravelersRecall)

The addon provides the user interface used to browse and select unlocked teleport locations.

The addon window can be opened using:

```text
/tr
```

or by clicking the minimap button.

---

## Installation

### 1. Clone the Module

Clone this repository into the `modules` directory of your AzerothCore source tree.

Example:

```bash
cd /path/to/azerothcore/modules
git clone https://github.com/NecroHome/mod-travelers-recall.git
```

### 2. Rebuild AzerothCore

After cloning the module, rebuild AzerothCore following the normal build process for your environment.

The exact build command may vary depending on your operating system and build configuration.

---

### 3. Apply the Database Updates

The module provides SQL updates for the following databases:

```text
data/world
data/characters
```

Apply the SQL files to their corresponding AzerothCore databases:

| Directory         | Database           |
| ----------------- | ------------------ |
| `data/world`      | `acore_world`      |
| `data/characters` | `acore_characters` |

Depending on your AzerothCore version and database updater configuration, these updates may be applied automatically when `worldserver` starts.

If automatic updates are not enabled or the updates are not applied automatically, execute the SQL files manually against the appropriate databases.

> Always verify the `worldserver` console output to confirm that the module's SQL updates were successfully applied.

---

### 4. Install the Module Configuration

The repository includes a default configuration template:

```text
mod-travelers-recall.conf.dist
```

Copy it into your AzerothCore `conf` directory and rename it to:

```text
mod-travelers-recall.conf
```

Example:

```bash
cp /path/to/azerothcore/env/dist/etc/modules/mod-travelers-recall.conf.dist
   /path/to/azerothcore/env/dist/etc/modules/mod-travelers-recall.conf
```

Your resulting structure should look similar to:

```text
azerothcore/
├── env
    ├── dist
        ├── etc
            ├── modules
                ├── mod-travelers-recall.conf
```

If your AzerothCore version automatically copies module configuration files during the build or installation process, verify whether the file has already been placed in the `conf` directory before copying it manually.

---

### 5. Install the Client Addon

Download the addon from its separate repository:

[**TravelersRecall — Client Addon**](https://github.com/NecroHome/TravelersRecall)

Install it into your World of Warcraft client directory.

The final directory structure should look similar to:

```text
World of Warcraft/
    └── Interface/
        └── AddOns/
            └── TravelersRecall/
```

The module was created for the WoW version 3.5.5

After installing the addon, restart the game client if necessary and verify that the addon is enabled on the character-selection screen.

---

## Customization

### Individual Cooldowns

Teleport locations are stored in:

```text
acore_world.custom_travelers_recall_locations
```

The `cooldown` column controls the cooldown for each individual location.

* `0`

  * Uses the global cooldown configured by the module
* Greater than `0`

  * Uses an individual cooldown value in seconds

Examples:

```text
3600  = 1 hour
7200  = 2 hours
86400 = 24 hours
```

Example:

```sql
UPDATE acore_world.custom_travelers_recall_locations
SET cooldown = 7200
WHERE name = 'Brill';
```

This configures Brill to use a two-hour cooldown.

---

### Adding Custom Locations

Custom teleport locations can be added directly to:

```text
acore_world.custom_travelers_recall_locations
```

Example:

```sql
INSERT IGNORE INTO acore_world.custom_travelers_recall_locations
(
    area_id,
    name,
    map_id,
    position_x,
    position_y,
    position_z,
    orientation,
    faction,
    icon,
    required_level,
    cooldown
)
VALUES
(
    159,
    'Brill',
    0,
    2259.25,
    290.43,
    34.1137,
    2.503233,
    2,
    'Interface\\Icons\\Spell_Arcane_TeleportUnderCity',
    0,
    0
);
```

> The `area_id` must correspond to the area where the player should unlock the teleport location.

---

### Column Explanation

| Column           | Description                                        |
| ---------------- | -------------------------------------------------- |
| `area_id`        | Area ID used to detect location discovery          |
| `name`           | Name displayed in the addon                        |
| `map_id`         | Destination map ID                                 |
| `position_x/y/z` | Destination coordinates                            |
| `orientation`    | Player facing direction after teleport             |
| `faction`        | Faction restriction                                |
| `icon`           | World of Warcraft icon path displayed in the addon |
| `required_level` | Reserved for future use                            |
| `cooldown`       | Individual cooldown in seconds                     |

---

### Faction Values

| Value | Faction  |
| ----- | -------- |
| `0`   | Neutral  |
| `1`   | Alliance |
| `2`   | Horde    |

---

### Retrieving Area Information

To retrieve the information required for a custom location, stand at the desired location in-game and execute:

```text
.gps
```

This provides information such as:

* Area ID
* Map ID
* Coordinates
* Orientation

Use these values when creating a custom teleport location.

---

## GM Commands

The module provides GM commands for testing and manually unlocking locations.

### Unlock a Specific Location

```text
.tr learn <area_id>
```

Example:

```text
.tr learn 159
```

Unlocks the location associated with the specified area ID for the selected character.

### Unlock All Locations

```text
.tr learn all
```

Unlocks all available Traveler's Recall locations for the selected character.

> These commands are useful for testing, administration, and verifying custom locations.

---

## Notes

* No `worldserver` restart is required after adding new locations to the database.
* Players who are already inside an area must leave and re-enter the area before the location is detected and unlocked.
* Icons use standard World of Warcraft client icon paths.
* The client addon is maintained separately from this module.
* Database changes should be made carefully, especially when modifying existing locations.

---

## Images

<img width="382" height="646" alt="image" src="https://github.com/user-attachments/assets/606e409e-e99a-411f-814f-87e14b324be0" />
<img width="388" height="647" alt="image" src="https://github.com/user-attachments/assets/07f8ad7c-f936-43d4-a463-8b1322fc0deb" />

## Author

NecroHome

---

## Change Log

### 29/05/2026

* Added new locations.
* Added GM commands:

  * `.tr learn <area_id>`
  * `.tr learn all`

### 20/05/2026

* Added bot party teleport support.
* Fixed location discovery while dead.
* Added Brill, Razor Hill, and Camp Taurajo.
