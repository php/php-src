--TEST--
Yield from by reference is not supported
--FILE--
<?php

function &gen() {
    yield from [];
}

?>
--EXPECTF--
Fatal error: Cannot use "yield from" inside a by-reference generator in %s on line %d
