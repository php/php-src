--TEST--
Unpacking non-array/Traversable detected at compile-time
--XFAIL--
Still need to check why this behavior changes
--FILE--
<?php

var_dump([...42]);

?>
--EXPECTF--
Fatal error: Only arrays and Traversables can be unpacked, int given in %s on line %d
