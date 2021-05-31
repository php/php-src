--TEST--
Collision between string and float literals during compaction
--EXTENSIONS--
opcache
--FILE--
<?php

var_dump("\0\0\0\0\0\0\0\0");
var_dump(0.0);
var_dump("\0\0\0\0\0\0\0\0");
var_dump(0.0);

?>
--EXPECTF--
string(8) "%0%0%0%0%0%0%0%0"
float(0)
string(8) "%0%0%0%0%0%0%0%0"
float(0)
