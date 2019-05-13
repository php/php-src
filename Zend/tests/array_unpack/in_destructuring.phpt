--TEST--
Spread operator is not supported in destructuring assignments
--FILE--
<?php

[$head, ...$tail] = [1, 2, 3];

?>
--EXPECT--
Fatal error: Spread operator is not supported in assignments in %s on line %d
