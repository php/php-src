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
Fatal error: Uncaught Error: Cannot create dynamic property Closure::$a in %s:%d
Stack trace:
#0 {main}
  thrown in %sclosure_022.php on line 5
