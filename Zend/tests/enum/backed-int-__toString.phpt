--TEST--
Backed int enum __toString is forbidden
--FILE--
<?php

enum Foo: int {
    case Bar = 0;

    public function __toString()
    {
        return '__toString';
    }
}

?>
--EXPECTF--
Fatal error: Enum may not include __toString in %s on line %d
