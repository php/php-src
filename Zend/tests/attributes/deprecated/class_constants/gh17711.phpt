--TEST--
GH-17711: Infinite recursion through deprecated class constants self-referencing through deprecation message
--FILE--
<?php

const TEST = 'Message';

class C {
    #[\Deprecated(self::C)]
    const C = TEST;
}

var_dump(C::C);

?>
--EXPECTF--
Deprecated: Constant C::C is deprecated, Message in %s on line %d
string(7) "Message"
