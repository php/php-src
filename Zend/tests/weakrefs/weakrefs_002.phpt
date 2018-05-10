--TEST--
Weakref no serialization
--FILE--
<?php
$wr = new WeakRef(new stdClass);

var_dump(serialize($wr));
?>
--EXPECTF--
Fatal error: Uncaught Exception: Serialization of 'WeakRef' is not allowed in %s:4
Stack trace:
#0 %s(4): serialize(Object(WeakRef))
#1 {main}
  thrown in %s on line 4
