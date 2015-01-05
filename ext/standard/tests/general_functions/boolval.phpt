--TEST--
Testing boolval()
--FILE--
<?php
	var_dump(boolval(false));
	var_dump(boolval(NULL));
	var_dump(boolval(""));
	var_dump(boolval(0));
	var_dump(boolval(array()));

	var_dump(boolval(true));
	var_dump(boolval("abc"));
	var_dump(boolval(0.5));
	var_dump(boolval(100));
	var_dump(boolval(new stdClass()));
	var_dump(boolval(STDIN));
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
