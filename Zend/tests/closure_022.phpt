--TEST--
Closure 022: Closure properties
--FILE--
<?php
$a = 0;
$foo = function() use ($a) {
};
$foo->a = 1;
?>
--EXPECTF--
Fatal error: Closure object cannot have properties in %sclosure_022.php on line 5

