--TEST--
Verify type rendering in type error for implicitly nullable intersection types
--FILE--
<?php

function foo(X&Y $foo = null) {
    var_dump($foo);
}

try {
    foo(5);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
foo(): Argument #1 ($foo) must be of type (X&Y)|null, int given, called in %s on line %d
