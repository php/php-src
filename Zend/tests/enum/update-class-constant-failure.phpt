--TEST--
Test failure of updating class constants
--FILE--
<?php

enum Foo: string {
    const Bar = NONEXISTENT;
}

var_dump(Foo::Bar);

?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "NONEXISTENT" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
