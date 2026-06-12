CREATE TABLE IF NOT EXISTS `custom_travelers_recall_locations` (
  `id` int NOT NULL AUTO_INCREMENT,
  `area_id` int NOT NULL,
  `name` varchar(100) COLLATE utf8mb4_unicode_ci NOT NULL,
  `map_id` int NOT NULL,
  `position_x` float NOT NULL,
  `position_y` float NOT NULL,
  `position_z` float NOT NULL,
  `orientation` float NOT NULL DEFAULT '0',
  `faction` tinyint NOT NULL DEFAULT '2',
  `icon` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `required_level` int DEFAULT '1',
  `cooldown` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `idx_area_id` (`area_id`)
);