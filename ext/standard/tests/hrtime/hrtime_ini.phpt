--TEST--
Ensure hrtime.resolution is not changeable from script
--INI--
hrtime.resolution=1000000
--FILE--
<?php
	var_dump(
		ini_set("hrtime.resolution", 0),
		ini_get("hrtime.resolution")
	);
?>
--EXPECT--
bool(false)
string(7) "1000000"

