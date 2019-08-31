--TEST--
Unpacking non-array/Traversable detected at compile-time
--FILE--
<?php

var_dump([...42]);

?>
--EXPECTF--
Fatal error: Only arrays and Traversables can be unpacked in %s on line %d
