--TEST--
putenv() basic tests
--FILE--
<?php

$var_name="SUCHVARSHOULDNOTEXIST";

var_dump(getenv($var_name));
var_dump(putenv($var_name."=value"));
var_dump(getenv($var_name));

var_dump(putenv($var_name."="));
var_dump(getenv($var_name));

var_dump(putenv($var_name));
var_dump(getenv($var_name));

echo "Done\n";
?>
--EXPECTF--	
bool(false)
bool(true)
string(5) "value"
bool(true)
string(0) ""
bool(true)
bool(false)
Done
