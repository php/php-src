--TEST--
Spread operator is not supported in destructuring assignments (only spread)
--FILE--
<?php

[...$x] = [1, 2, 3];

?>
--EXPECTF--
Fatal error: Spread operator is not supported in assignments in %s on line %d
