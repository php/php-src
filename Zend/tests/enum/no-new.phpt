--TEST--
Enum no new
--FILE--
<?php

enum Foo {}

try {
    new Foo();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot instantiate enum Foo
