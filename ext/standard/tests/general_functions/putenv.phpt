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

var_dump(putenv("=123"));
var_dump(putenv(""));

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

Warning: putenv(): Invalid parameter syntax in %s on line %d
bool(false)

Warning: putenv(): Invalid parameter syntax in %s on line %d
bool(false)
Done
