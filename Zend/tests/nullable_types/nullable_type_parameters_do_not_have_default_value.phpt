--TEST--
Explicit nullable types do not imply a default value

--FILE--
<?php

function f(?callable $p) {}

f();

--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to f() must be callable, none given, called in %s on line %d and defined in %s:%d
Stack trace:
#%d %s
#%d %s
  thrown in %s on line %d

