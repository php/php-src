--TEST--
Cannot assign to $GLOBALS
--FILE--
<?php

$GLOBALS = [];

?>
--EXPECTF--
Fatal error: $GLOBALS can only be modified using the $GLOBALS[$name] = $value syntax in %s on line %d
