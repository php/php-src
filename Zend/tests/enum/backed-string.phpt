--TEST--
String enum value
--FILE--
<?php

enum Foo: string {
    case Bar = 'bar';
    case Baz = 'baz';
}

echo Foo::Bar->value . "\n";
echo Foo::Baz->value . "\n";

?>
--EXPECT--
bar
baz
