--TEST--
Weakref no references
--FILE--
<?php
$wr = new WeakRef(new stdClass);

$disallow = &$wr->disallowed;
?>
--EXPECTF--
Fatal error: Uncaught Error: weakref objects do not support property references in %s:4
Stack trace:
#0 {main}
  thrown in %s on line 4
