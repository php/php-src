--TEST--
SPL: iterator_any() array argument exception test
--FILE--
<?php
iterator_any([]);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to iterator_any() must implement interface Traversable, array given in %s:%d
Stack trace:
#0 %s(%d): iterator_any(Array)
#1 {main}
  thrown in %s on line %d
