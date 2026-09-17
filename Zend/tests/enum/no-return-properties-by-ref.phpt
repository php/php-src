--TEST--
Enum properties cannot be returned by-ref
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
}

function &getBarValueByRef() {
    $bar = Foo::Bar;
    return $bar->value;
}

try {
    $value = &getBarValueByRef();
    $value = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(Foo::Bar->value);

?>
--EXPECT--
Error: Cannot indirectly modify readonly property Foo::$value
int(0)
