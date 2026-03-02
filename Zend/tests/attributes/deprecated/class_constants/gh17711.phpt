--TEST--
GH-17711: Infinite recursion through deprecated class constants self-referencing through deprecation message
--FILE--
<?php

class C {
    #[\Deprecated(self::C)]
    const C = TEST;
}

const TEST = 'Message';
var_dump(C::C);

class D {
    #[\Deprecated(Alias::C)]
    const C = 'test';
}

class_alias('D', 'Alias');
var_dump(D::C);

?>
--EXPECTF--
Deprecated: Constant C::C is deprecated, Message in %s on line %d
string(7) "Message"

Deprecated: Constant D::C is deprecated, test in %s on line %d
string(4) "test"
