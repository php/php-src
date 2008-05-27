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
unicode(5) "value"
bool(true)
unicode(0) ""
bool(true)
bool(false)
Done
