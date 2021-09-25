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
} catch (\Error $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Cannot create dynamic property Foo::$baz
