SET @exists = (
    SELECT COUNT(*)
    FROM information_schema.statistics
    WHERE table_schema = 'acore_characters'
      AND table_name = 'custom_travelers_recall_unlocks'
      AND index_name = 'uq_guid_location'
);

SET @sql = IF(
    @exists = 0,
    'ALTER TABLE acore_characters.custom_travelers_recall_unlocks ADD UNIQUE KEY uq_guid_location (guid, location_id)',
    'SELECT "Index already exists"'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;