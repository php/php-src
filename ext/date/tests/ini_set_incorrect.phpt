--TEST--
Test invalid time zone passed to ini_set
--FILE--
<?php

ini_set("date.timezone", "Incorrect/Zone");

?>
--EXPECTF--
Warning: ini_set(): Invalid date.timezone value 'Incorrect/Zone', we selected the timezone 'UTC' for now. in %sini_set_incorrect.php on line %d
