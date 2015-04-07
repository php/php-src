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
Exception: foo in %s on line %d
Stack trace:
#0 %s(%d): gen()
#1 {main}

