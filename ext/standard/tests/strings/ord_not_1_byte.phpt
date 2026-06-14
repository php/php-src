--TEST--
ord() with values not one byte long
--FILE--
<?php

var_dump(ord(""));
var_dump(ord("Hello"));

?>
--EXPECTF--
Deprecated: ord(): Providing an empty string is deprecated in %s on line 3
int(0)

Deprecated: ord(): Providing a string that is not one byte long is deprecated. Use ord($str[0]) instead in %s on line 4
int(72)
