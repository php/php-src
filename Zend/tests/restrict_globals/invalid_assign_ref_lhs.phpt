--TEST--
Cannot by-ref assign to $GLOBALS (LHS)
--FILE--
<?php

$var = [];
$GLOBALS =& $var;

?>
--EXPECTF--
Fatal error: $GLOBALS can only be modified using the $GLOBALS[$name] = $value syntax in %s on line %d
