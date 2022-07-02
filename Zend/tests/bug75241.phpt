--TEST--
Bug #75241 (Null pointer dereference in zend_mm_alloc_small())
--FILE--
<?php

$d->d = &$d + $d->d/=0;
var_dump($d);
?>
--EXPECTF--
Fatal error: Uncaught Error: Attempt to modify property "d" on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
