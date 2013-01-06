--TEST--
Bug #55705 (Omitting a callable typehinted argument causes a segfault)
--FILE--
<?php
function f(callable $c) {}
f();
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to f() must be callable, none given, called in %s on line 3 and defined in %s on line %d
