--TEST--
Test invalid time zone and defaults
--INI--
date.timezone=UTC
--FILE--
<?php
echo ini_get("date.timezone"), "\n";

ini_set("date.timezone", "foo");
echo ini_get("date.timezone"), "\n";

ini_set("date.timezone", "Europe/London");
echo ini_get("date.timezone"), "\n";

ini_set("date.timezone", "Mars/Valles_Marineris");
echo ini_get("date.timezone"), "\n";
?>
--EXPECTF--
UTC

Warning: ini_set(): Invalid date.timezone value 'foo', using 'UTC' instead in %sini_set_incorrect-002.php on line %d
UTC
Europe/London

Warning: ini_set(): Invalid date.timezone value 'Mars/Valles_Marineris', using 'Europe/London' instead in %sini_set_incorrect-002.php on line %d
Europe/London
