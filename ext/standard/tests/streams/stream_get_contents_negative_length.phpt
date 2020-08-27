--TEST--
stream_get_contents() with negative max length
--FILE--
<?php

$tmp = tmpfile();
fwrite($tmp, "abcd");
var_dump(stream_get_contents($tmp, 2, 1));
var_dump(stream_get_contents($tmp, -2));

?>
--EXPECTF--
string(2) "bc"

Warning: stream_get_contents(): Length must be greater than or equal to zero, or -1 in %s on line %d
bool(false)
