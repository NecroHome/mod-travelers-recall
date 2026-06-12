CREATE TABLE IF NOT EXISTS `custom_travelers_recall_unlocks` (
  `guid` int NOT NULL,
  `location_id` int NOT NULL,
  `unlocked_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `cooldown_end` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`location_id`)
);