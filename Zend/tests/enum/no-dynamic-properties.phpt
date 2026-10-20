--TEST--
Enum case disallows dynamic properties
--FILE--
<?php

enum Foo {
    case Bar;
}

$bar = Foo::Bar;

try {
    $bar->baz = 'Baz';
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot create dynamic property Foo::$baz
