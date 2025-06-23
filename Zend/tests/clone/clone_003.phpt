--TEST--
Using clone statement on undefined variable
--FILE--
<?php

try {
    $a = clone $b;
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Warning: Undefined variable $b in %s on line %d
TypeError: clone(): Argument #1 ($object) must be of type object, null given
