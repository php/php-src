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
Catchable fatal error: Closure object cannot have properties in %s22.php on line 5

