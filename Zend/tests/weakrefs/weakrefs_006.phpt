--TEST--
Weakref no isset
--FILE--
<?php
$wr = new WeakRef(new stdClass);

isset($wr->disallow);
?>
--EXPECTF--
Fatal error: Uncaught Error: weakref objects do not support properties in %s:4
Stack trace:
#0 {main}
  thrown in %s on line 4
