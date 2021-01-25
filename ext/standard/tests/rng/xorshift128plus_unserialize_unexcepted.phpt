--TEST--
Test class: XorShift128Plus: unexcepted __unserialize().
--FILE--
<?php

$rng = \unserialize('O:19:"RNG\XorShift128Plus":3:{i:0;s:1:"0";i:1;s:1:"0";i:2;a:0:{}}');

?>
--EXPECTF--
Fatal error: Uncaught Exception: Incomplete or ill-formed serialization data in %s:%i
Stack trace:
#0 [internal function]: %s->__unserialize(Array)
#1 %s(%i): unserialize('%s')
#2 {main}
  thrown in %s on line %i
