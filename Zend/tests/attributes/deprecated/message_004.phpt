--TEST--
#[\Deprecated]: Message with value unknown at compile time.
--FILE--
<?php

define('MESSAGE', 'value-' . (random_int(1, 2) == 1 ? 'a' : 'b'));

#[\Deprecated(MESSAGE)]
function test() {
}

test();

?>
--EXPECTF--
Deprecated: Function test() is deprecated, value-%c in %s on line %d
