--TEST--
SPL: iterator_every() bool argument exception test
--FILE--
<?php
iterator_every(true);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to iterator_every() must implement interface Traversable, bool given in %s:%d
Stack trace:
#0 %s(%d): iterator_every(true)
#1 {main}
  thrown in %s on line %d
