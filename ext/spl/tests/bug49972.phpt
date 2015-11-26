--TEST--
Bug #49972 (AppendIterator undefined function crash)
--FILE--
<?php

$iterator = new AppendIterator();
$iterator->undefined();

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method AppendIterator::undefined() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
