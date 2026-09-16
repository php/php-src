--TEST--
Dynamic class constant fetch in constant expressions
--FILE--
<?php

class Foo {
    public const BAR = 'bar';
    public const BA = 'BA';
    public const R = 'R';
    public const CLASS_ = 'class';
    public const A = self::{'BAR'};
    public const B = self::{'BA' . 'R'};
    public const C = self::{self::BA . self::R};
}

var_dump(Foo::A);
var_dump(Foo::B);
var_dump(Foo::C);

?>
--EXPECT--
string(3) "bar"
string(3) "bar"
string(3) "bar"
