--TEST--
Generator return type with wrong case is not generator-compatible
--FILE--
<?php
function gen(): iterator {
    yield 1;
}
?>
--EXPECTF--
Fatal error: Generator return type must be a supertype of Generator, iterator given in %s on line %d
