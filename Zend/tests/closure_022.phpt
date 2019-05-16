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
Fatal error: Uncaught Error: Closure object cannot have properties in %sclosure_022.php:5
Stack trace:
#0 {main}
  thrown in %sclosure_022.php on line 5
