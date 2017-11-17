--TEST--
Bug #55705 (Omitting a callable typehinted argument causes a segfault)
--FILE--
<?php
function f(callable $c) {}
f();
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: Too few arguments to function f(), 0 passed in %s on line 3 and exactly 1 expected in %s:2
Stack trace:
#0 %s(%d): f()
#1 {main}
  thrown in %s on line %d
