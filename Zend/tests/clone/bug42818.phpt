--TEST--
Bug #42818 ($foo = clone(array()); leaks memory)
--FILE--
<?php
$foo = clone(array());
?>
--EXPECTF--
Fatal error: Uncaught TypeError: clone(): Argument #1 ($object) must be of type object, array given in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug42818.php on line 2
