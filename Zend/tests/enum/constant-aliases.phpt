--TEST--
Enum constants can alias cases
--FILE--
<?php

enum Foo {
    case Bar;
    const Baz = self::Bar;
}

function test(Foo $var) {
    echo "works\n";
}

test(Foo::Bar);
test(Foo::Baz);

?>
--EXPECT--
works
works
