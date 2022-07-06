--TEST--
Non-ref generators cannot be iterated by-ref
--FILE--
<?php

function gen() { yield; }

$gen = gen();
foreach ($gen as &$value) { }

?>
--EXPECTF--
Fatal error: Uncaught Exception: You can only iterate a generator by-reference if it declared that it yields by-reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
