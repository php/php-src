--TEST--
Exceptions throwing by generators during foreach iteration are properly handled
--FILE--
<?php

function gen() {
    throw new Exception("foo");
    yield; // force generator
}

foreach (gen() as $value) { }

?>
--EXPECTF--
Fatal error: Uncaught Exception: foo in %s:%d
Stack trace:
#0 %s(%d): gen()
#1 {main}
  thrown in %s on line %d
