--TEST--
Test nullsafe in binary op
--FILE--
<?php

$foo = null;

var_dump($foo?->foo() + $foo?->bar());
var_dump(($foo?->foo() + $foo?->bar())?->baz());

?>
--EXPECTF--
int(0)

Fatal error: Uncaught Error: Call to a member function baz() on int in %s:6
Stack trace:
#0 {main}
  thrown in %s on line 6
