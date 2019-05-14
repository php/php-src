--TEST--
Cannot unpack in array with keys
--FILE--
<?php

$ary = [1, 2, 3];
$ary2 = [42 => 0, ...$ary];

?>
--EXPECTF--
Fatal error: Cannot use unpacking in arrays with explicit keys in %s on line %d
