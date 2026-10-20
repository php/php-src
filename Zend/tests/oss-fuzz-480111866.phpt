--TEST--
OSS-Fuzz #480111866: Assignment to assignment through list operator
--FILE--
<?php

[[&$y] = y] = y;

?>
--EXPECTF--
Fatal error: Assignments can only happen to writable values in %s on line %d
