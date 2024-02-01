--TEST--
Test invalid time zone passed to ini_set
--INI--
date.timezone=UTC
--FILE--
<?php

ini_set("date.timezone", "Incorrect/Zone");

?>
--EXPECTF--
Warning: ini_set(): Invalid date.timezone value 'Incorrect/Zone', using 'UTC' instead in %sini_set_incorrect.php on line %d
