--TEST--
Cannot unpack in array with keys (2)
--FILE--
<?php

$ary = [1, 2, 3];
$ary2 = [...$ary, 'a' => 4];

?>
--EXPECTF--
Fatal error: Cannot use unpacking in arrays with explicit keys in %s on line %d
