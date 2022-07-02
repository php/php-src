--TEST--
Cannot unset $GLOBALS
--FILE--
<?php

unset($GLOBALS);

?>
--EXPECTF--
Fatal error: $GLOBALS can only be modified using the $GLOBALS[$name] = $value syntax in %s on line %d
