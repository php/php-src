--TEST--
filter_var() & FILTER_VALIDATE_IP and weird data
--EXTENSIONS--
filter
--FILE--
<?php

var_dump(filter_var("....", FILTER_VALIDATE_IP));
var_dump(filter_var("...", FILTER_VALIDATE_IP));
var_dump(filter_var("..", FILTER_VALIDATE_IP));
var_dump(filter_var(".", FILTER_VALIDATE_IP));
var_dump(filter_var("1.1.1.1", FILTER_VALIDATE_IP));

echo "Done\n";
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
string(7) "1.1.1.1"
Done
