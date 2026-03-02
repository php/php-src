--TEST--
#[\Deprecated]: Message is empty.
--FILE--
<?php

#[\Deprecated("")]
function test() {
}

test();

?>
--EXPECTF--
Deprecated: Function test() is deprecated in %s on line %d
