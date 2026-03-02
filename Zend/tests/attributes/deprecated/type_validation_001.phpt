--TEST--
#[\Deprecated]: Type validation of $message parameter with int.
--FILE--
<?php

#[\Deprecated(1234)]
function test() {
}

test();

?>
--EXPECTF--
Deprecated: Function test() is deprecated, 1234 in %s on line %d
