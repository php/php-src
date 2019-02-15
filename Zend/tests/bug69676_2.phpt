--TEST--
Bug #69676: Resolution of self::FOO in class constants not correct (variation)
--FILE--
<?php

class Foo {
    const A = 'Foo::A';
    const B = self::A . ' and ' . self::C;
    const C = 'Foo::C';

}

class Bar extends Foo {
    const A = 'Bar::A';
    const C = 'Bar::C';
}

var_dump(Bar::B);

?>
--EXPECT--
string(17) "Foo::A and Foo::C"
