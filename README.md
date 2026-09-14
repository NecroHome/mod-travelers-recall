# Traveler's Recall Module

Progressive fast-travel system for AzerothCore.

---

## For AzerothCore Playerbots

This module was originally created for the **AzerothCore Playerbots** branch.

It may or may not work with standard AzerothCore ("Vanilla").

If you are using the standard AzerothCore branch, use the dedicated version instead:

[**Mod Traveler's Recall - Vanilla**](https://github.com/NecroHome/mod-travelers-recall/tree/main-vanilla)

---

## 13/09/2026 UPDATE

**Important:** If you update the module to this version, make sure to update the client addon as well.

---

## Description

Traveler's Recall is a progressive teleport system for AzerothCore.

Players unlock teleport locations by discovering cities and important places throughout the world. Once a location has been unlocked, it becomes available through the Traveler's Recall client addon.

The module handles teleport logic, unlock progression, cooldowns, faction restrictions, and player-state validation.

---

## Features

* Unlock teleport locations by discovering cities and important places
* Custom teleport window integration
* Global or individual teleport cooldowns
* Faction restrictions
* Player state protections
* Custom unlockable locations
* Bot party teleport support
* Level requirement support

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

## 1. Installation

### 1.1. Clone the Module

Clone this repository into the `modules` directory of your AzerothCore source tree.

Example:

```bash
cd /path/to/azerothcore/modules
git clone https://github.com/NecroHome/mod-travelers-recall.git
```

### 1.2. Rebuild AzerothCore

After cloning the module, rebuild AzerothCore following the normal build process for your environment.

The exact build command may vary depending on your operating system and build configuration.

---

### 1.3. Apply the Database Updates

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

---

### 1.4. Install the Module Configuration

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
cp /path/to/azerothcore/modules/mod-travelers-recall.conf.dist \
   /path/to/azerothcore/env/dist/etc/modules/mod-travelers-recall.conf
```

Your resulting structure should look similar to:

```text
azerothcore/
└── env/
    └── dist/
        └── etc/
            └── modules/
                └── mod-travelers-recall.conf
```

If your AzerothCore version automatically copies module configuration files during the build or installation process, verify whether the file has already been placed in the `env/dist/etc/modules` directory before copying it manually.

---

### 1.5. Install the Client Addon

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

The module was created for WoW version **3.5.5**.

After installing the addon, restart the game client if necessary and verify that the addon is enabled on the character-selection screen.

---

## 2. Customization

### 2.1. Individual Cooldowns

Teleport locations are stored in:

```text
acore_world.custom_travelers_recall_locations
```

The `cooldown` column controls the cooldown for each individual location.

* `0`
  * Uses the global cooldown configured by the module
  * If `TravelersRecall.UseDefaultCooldown` is set to `1` in the configuration file, all locations will have a default cooldown configured by `TravelersRecall.DefaultCooldown`.
* Greater than `0`
  * Uses an individual cooldown value in seconds

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

### 2.2. Level Requirement

The `required_level` column controls the required level for each individual location.

* `0`

  * Location has no level requirement
* Greater than `0`

  * If `TravelersRecall.UseRequiredLevel` is set to `1` in the configuration file, players must be at least the specified level to unlock that location.

Example:

```sql
UPDATE acore_world.custom_travelers_recall_locations
SET required_level = 10
WHERE area_id = 159;
```

This configures Brill to require at least level 10 to unlock the teleport location.

---

### 2.3. Faction Restriction

The `faction` column controls the faction restriction for that location.

* `0`

  * Location is `Neutral territory` and both factions can unlock it.
* `1`

  * Location is `Alliance territory`. Only Alliance players can unlock it.
* `2`

  * Location is `Horde territory`. Only Horde players can unlock it.

Example:

```sql
UPDATE acore_world.custom_travelers_recall_locations
SET faction = 0
WHERE area_id = 159;
```

This configures Brill as a `Neutral territory`, allowing both Alliance and Horde players to unlock it.

---

### 2.4. Adding Custom Locations

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

### 2.5. Column Explanation

| Column           | Description                                        |
| ---------------- | -------------------------------------------------- |
| `area_id`        | Area ID used to detect location discovery          |
| `name`           | Name displayed in the addon                        |
| `map_id`         | Destination map ID                                 |
| `position_x/y/z` | Destination coordinates                            |
| `orientation`    | Player facing direction after teleport             |
| `faction`        | Faction restriction                                |
| `icon`           | World of Warcraft icon path displayed in the addon |
| `required_level` | Minimum level required to unlock the location      |
| `cooldown`       | Individual cooldown in seconds                     |

---

### 2.6. Retrieving Area Information

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

## 3. GM Commands

The module provides GM commands for testing and manually unlocking locations.

### 3.1. Unlock a Specific Location

```text
.tr learn <area_id>
```

Example:

```text
.tr learn 159
```

This unlocks `Brill` for the selected player or yourself.

### 3.2. Unlock All Locations

```text
.tr learn all
```

Unlocks all available Traveler's Recall locations for the selected character.

### 3.3. Remove a Specific Location

```text
.tr remove <area_id>
```

Example:

```text
.tr remove 159
```

This removes `Brill` from the selected player or yourself.

### 3.4. Remove All Locations

```text
.tr remove all
```

Removes all Traveler's Recall locations from the selected player or yourself.

---

## 4. Notes

* No `worldserver` restart is required after adding new locations to the database.
* No `worldserver` restart is required when updating faction or required level, but players who already have that location unlocked will keep it.
* Players who are already inside an area must leave and re-enter the area before the location is detected and unlocked.
* Icons use standard World of Warcraft client icon paths.

---

## 5. Images

<img width="382" height="646" alt="image" src="https://github.com/user-attachments/assets/606e409e-e99a-411f-814f-87e14b324be0" />
<img width="388" height="647" alt="image" src="https://github.com/user-attachments/assets/07f8ad7c-f936-43d4-a463-8b1322fc0deb" />
<img width="365" height="124" alt="image" src="https://github.com/user-attachments/assets/ce9a2e54-49b0-42f1-925c-bc1e58f583ed" />

---

## 6. FAQ

Q: Can I contact you?

A: Yes!

Q: I have an idea for the module!

A: That's not a question, but feel free to open a feature request!

---

## 7. Author

NecroHome

---

## 8. Change Log

### 13/09/2026

* Added Required Level support
* Added GM commands:

  * `.tr remove <area_id>`
  * `.tr remove all`

# Game Addon must be updated

### 29/05/2026

* Added new locations.
* Added GM commands:

  * `.tr learn <area_id>`
  * `.tr learn all`

### 20/05/2026

* Added bot party teleport support.
* Fixed location discovery while dead.
* Added Brill, Razor Hill, and Camp Taurajo.
