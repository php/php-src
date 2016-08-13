--TEST--
Bug #42817 (clone() on a non-object does not result in a fatal error)
--FILE--
<?php
$a = clone(null);
array_push($a->b, $c);
?>
--EXPECTF--
Fatal error: Uncaught Error: __clone method called on non-object in %sbug42817.php:2
Stack trace:
#0 {main}
  thrown in %sbug42817.php on line 2
