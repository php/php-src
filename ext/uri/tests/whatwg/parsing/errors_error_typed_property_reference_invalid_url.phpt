--TEST--
Test Uri\WhatWg\Url parsing - errors - assignment to a typed property reference for an invalid URL
--FILE--
<?php

class Foo {
    public string $x = '';
}

$foo = new Foo();

try {
    Uri\WhatWg\Url::parse('invalid uri', errors: $foo->x);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot assign array to reference held by property Foo::$x of type string
