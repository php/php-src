--TEST--
Cannot list-assign to $GLOBALS
--FILE--
<?php

list($GLOBALS) = [1];

?>
--EXPECTF--
Fatal error: $GLOBALS can only be modified using the $GLOBALS[$name] = $value syntax in %s on line %d
