--TEST--
SPL: iterator_any() bool argument exception test
--FILE--
<?php
iterator_any(true);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to iterator_any() must implement interface Traversable, bool given in %s:%d
Stack trace:
#0 %s(%d): iterator_any(true)
#1 {main}
  thrown in %s on line %d
