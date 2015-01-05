--TEST--
Bug #42818 ($foo = clone(array()); leaks memory)
--FILE--
<?php
$foo = clone(array());
?>
--EXPECTF--
Fatal error: __clone method called on non-object in %sbug42818.php on line 2

