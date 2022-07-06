--TEST--
Unpacking of string keys detected at compile-time
--FILE--
<?php

var_dump([...['a' => 'b']]);

?>
--EXPECTF--
Fatal error: Cannot unpack array with string keys in %s on line %d
