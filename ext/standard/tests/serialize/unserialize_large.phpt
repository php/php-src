--TEST--
Unserializing payload with unrealistically large element counts
--FILE--
<?php

var_dump(unserialize("a:1000000000:{}"));
var_dump(unserialize("O:1000000000:\"\":0:{}"));
var_dump(unserialize("O:1:\"X\":1000000000:{}"));
var_dump(unserialize("C:1:\"X\":1000000000:{}"));

?>
--EXPECTF--
Notice: unserialize(): Error at offset 14 of 15 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 2 of 20 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 18 of 21 bytes in %s on line %d
bool(false)

Warning: Insufficient data for unserializing - 1000000000 required, 1 present in %s on line %d

Notice: unserialize(): Error at offset 20 of 21 bytes in %s on line %d
bool(false)
