--TEST--
Assignment to invalid list() value
--FILE--
<?php

[42] = [1];

?>
--EXPECTF--
Fatal error: Assignments can only happen to writable values in %s on line %d
