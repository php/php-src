--TEST--
Cannot list-assign to $GLOBALS (by-ref)
--FILE--
<?php

list(&$GLOBALS) = [1];

?>
--EXPECTF--
Fatal error: Cannot assign reference to non referenceable value in %s on line %d
