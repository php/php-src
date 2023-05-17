--TEST--
::class on an expression inside constant expressions must be object
--FILE--
<?php

class Foo {}

const A = (new Foo())::class;
var_dump(A);

const B = [0]::class;

?>
--EXPECTF--
string(3) "Foo"

Fatal error: Uncaught TypeError: Cannot use "::class" on array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
