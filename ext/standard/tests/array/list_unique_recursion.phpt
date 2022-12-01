--TEST--
Test List\unique() and recursion
--FILE--
<?php

$a = [];
$b = [&$a];
$a[0] = &$b;
List\unique([$a, $b]);

?>
--EXPECTF--
Fatal error: Nesting level too deep - recursive dependency? in %s on line %d
