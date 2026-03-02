--TEST--
Enum case is shown in stack trace
--FILE--
<?php

enum Foo {
    case Bar;
}

function test($enum) {
    throw new Exception();
}

test(Foo::Bar);

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): test(Foo::Bar)
#1 {main}
  thrown in %s on line %d
