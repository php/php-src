--TEST--
Test assigning errors by reference fails
--EXTENSIONS--
uri
--FILE--
<?php

class Foo {
	public string $x = '';
}

$f = new Foo();
try {
    Uri\WhatWg\Url::parse(" https://example.org ", errors: $f->x);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Cannot assign array to reference held by property Foo::$x of type string
