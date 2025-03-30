--TEST--
nested enum
--FILE--
<?php

class Outer {
    enum Inner {
        case Foo;
        case Bar;
    }
}

echo Outer\Inner::Foo->name . "\n";
?>
--EXPECT--
Foo
