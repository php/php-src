--TEST--
Spread operator is not supported in destructuring assignments
--FILE--
<?php

[$head, ...$tail] = [1, 2, 3];

?>
--EXPECTF--
Fatal error: Spread operator is not supported in assignments in %s on line %d
