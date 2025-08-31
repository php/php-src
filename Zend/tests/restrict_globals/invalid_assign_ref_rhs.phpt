--TEST--
Cannot by-ref assign to $GLOBALS (RHS)
--FILE--
<?php

$var = [];
$var =& $GLOBALS;

?>
--EXPECTF--
Fatal error: Cannot acquire reference to $GLOBALS in %s on line %d
