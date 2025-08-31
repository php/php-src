--TEST--
Cannot append to $GLOBALS
--FILE--
<?php

$GLOBALS[] = 1;

?>
--EXPECTF--
Fatal error: Cannot append to $GLOBALS in %s on line %d
