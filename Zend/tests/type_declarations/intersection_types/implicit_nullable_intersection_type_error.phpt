--TEST--
Verify type rendering in type error for implicitly nullable intersection types
--FILE--
<?php

function foo(X&Y $foo = null) {
    var_dump($foo);
}

try {
    foo(5);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: foo(): Implicitly marking parameter $foo as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d
TypeError: foo(): Argument #1 ($foo) must be of type (X&Y)|null, int given, called in %s on line %d
