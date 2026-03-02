--TEST--
Using clone statement on non-object
--FILE--
<?php

try {
    $a = clone array();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: clone(): Argument #1 ($object) must be of type object, array given
