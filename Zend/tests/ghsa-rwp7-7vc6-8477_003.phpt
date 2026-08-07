--TEST--
GHSA-rwp7-7vc6-8477: Use-after-free for ??= due to incorrect live-range calculation
--FILE--
<?php

class Foo {
    public int $prop;
}

function newFoo() {
    return new Foo();
}

try {
    newFoo()->prop ??= 'foo';
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Cannot assign string to property Foo::$prop of type int
