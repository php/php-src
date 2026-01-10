--TEST--
#[\Deprecated]: Message from constant.
--FILE--
<?php

#[\Deprecated(DEPRECATION_MESSAGE)]
function test() {
}

define('DEPRECATION_MESSAGE', 'from constant');

test();

?>
--EXPECTF--
Deprecated: Function test() is deprecated, from constant in %s on line %d
