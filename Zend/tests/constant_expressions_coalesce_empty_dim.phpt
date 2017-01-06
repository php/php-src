--TEST--
Constant expressions with empty dimension fetch on coalesce
--FILE--
<?php

const A = [][] ?? 1;

?>
--EXPECTF--
Fatal error: Cannot use [] for reading in %s.php on line %d

