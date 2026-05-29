-- Fixing The Exodar location
UPDATE acore_world.custom_travelers_recall_locations
SET area_id = 3557, position_x = -3965.7, position_y = -11653.6, position_z = -138.844, orientation = 0.852154
WHERE area_id = 1357 and map_id = 530;

-- Removing duplicates
USE acore_world;

DELETE t1
FROM custom_travelers_recall_locations t1
INNER JOIN custom_travelers_recall_locations t2
ON t1.area_id = t2.area_id AND t1.id > t2.id;

-- New unique key, area_id
DROP INDEX IF EXISTS acore_world.custom_travelers_recall_locations_unique
ON custom_travelers_recall_locations;

ALTER TABLE acore_world.custom_travelers_recall_locations
ADD UNIQUE KEY custom_travelers_recall_locations_unique (area_id);