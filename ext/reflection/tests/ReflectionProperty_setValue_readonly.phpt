--TEST--
Test ReflectionProperty::setValue() error cases.
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
}

$reflection = new ReflectionProperty(Foo::class, 'value');

try {
    $reflection->setValue(Foo::Bar, 1);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Foo::Bar->value);

?>
--EXPECT--
Cannot modify readonly property Foo::$value
int(0)
