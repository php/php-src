--TEST--
#[\Deprecated]: Message contains NUL bytes.
--FILE--
<?php

#[\Deprecated("Here is a NUL \x00 Byte")]
function test() {
}

test();

?>
--EXPECTF--
Deprecated: Function test() is deprecated, Here is a NUL %0 Byte in %s on line %d
