--TEST--
Match errors show unmatched enum case
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
}

try {
    match (Foo::Bar) {
        Foo::Baz => 42,
    };
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
UnhandledMatchError: Unhandled match case Foo::Bar
