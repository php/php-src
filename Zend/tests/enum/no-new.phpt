--TEST--
Enum no new
--FILE--
<?php

enum Foo {}

try {
    new Foo();
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Cannot instantiate enum Foo
