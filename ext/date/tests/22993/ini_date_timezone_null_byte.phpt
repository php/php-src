--TEST--
GH-22993: ini_set() with embedded NUL byte in date.timezone
--FILE--
<?php

ini_set("date.timezone", "foo\0bar");

?>
--EXPECTF--
Warning: ini_set(): Invalid date.timezone value 'foo%0bar', using 'UTC' instead in %s on line %d
