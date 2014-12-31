--TEST--
Writing to a temporary expression is not allowed
--FILE--
<?php

[0, 1][0] = 1;

?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
