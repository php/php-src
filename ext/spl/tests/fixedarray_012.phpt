--TEST--
SPL: FixedArray: Assigning the object to another variable using []
--FILE--
<?php

$a = new SplFixedArray(100);
$b = &$a[];

?>
--EXPECTF--
Notice: Indirect modification of overloaded element of SplFixedArray has no effect in %s on line %d

