--TEST--
Throwing exception using a class that isn't derived from the Exception base class
--FILE--
<?php

class Foo { }

try {
    throw new Foo();
} catch (Foo $e) {
    var_dump($e);
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot throw objects that do not implement Throwable in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
