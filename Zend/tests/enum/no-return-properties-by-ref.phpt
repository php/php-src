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
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Foo::Bar->value);

?>
--EXPECT--
Cannot modify readonly property Foo::$value
int(0)
