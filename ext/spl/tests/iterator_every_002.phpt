--TEST--
SPL: iterator_every() array argument exception test
--FILE--
<?php
iterator_every([]);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to iterator_every() must implement interface Traversable, array given in %s:%d
Stack trace:
#0 %s(%d): iterator_every(Array)
#1 {main}
  thrown in %s on line %d
