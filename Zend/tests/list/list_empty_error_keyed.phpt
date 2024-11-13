--TEST--
Cannot use empty elements in keyed array destructuring
--FILE--
<?php

$array = ['a' => 1, 'b' => 2];
['a' => $a, , 'b' => $b] = $array;

?>
--EXPECTF--
Fatal error: Cannot use empty array entries in keyed array assignment in %s on line %d
