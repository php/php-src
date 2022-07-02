--TEST--
Cannot use $GLOBALS as foreach result variable (by-ref)
--FILE--
<?php

foreach ([1] as &$GLOBALS) {}

?>
--EXPECTF--
Fatal error: $GLOBALS can only be modified using the $GLOBALS[$name] = $value syntax in %s on line %d
