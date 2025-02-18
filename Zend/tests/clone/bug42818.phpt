--TEST--
Bug #42818 ($foo = clone(array()); leaks memory)
--FILE--
<?php
$foo = clone(array());
?>
--EXPECTF--
Fatal error: Uncaught Error: __clone method called on non-object in %sbug42818.php:2
Stack trace:
#0 {main}
  thrown in %sbug42818.php on line 2
