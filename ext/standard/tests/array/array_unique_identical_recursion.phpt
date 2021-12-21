--TEST--
Test array_unique() with ARRAY_UNIQUE_IDENTICAL and recursion
--FILE--
<?php

$a = [];
$b = [&$a];
$a[0] = &$b;
array_unique([$a, $b], ARRAY_UNIQUE_IDENTICAL);

?>
--EXPECTF--
Fatal error: Nesting level too deep - recursive dependency? in %s on line %d
