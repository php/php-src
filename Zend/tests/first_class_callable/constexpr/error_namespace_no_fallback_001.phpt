--TEST--
FCC in initializer errors for missing function with FQN matching global function.
--FILE--
<?php

namespace Foo;

const Closure = \Foo\strrev(...);

var_dump(Closure);
var_dump((Closure)("abc"));

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function Foo\strrev() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
