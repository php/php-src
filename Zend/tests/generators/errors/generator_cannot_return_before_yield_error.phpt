--TEST--
Generators cannot return values (even before yield)
--FILE--
<?php

function gen() {
    return $foo;
    yield;
}

?>
--EXPECTF--
Fatal error: Generators cannot return values using "return" in %s on line 4
