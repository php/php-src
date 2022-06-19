--TEST--
Enum __toString
--FILE--
<?php

enum Foo {
    case Bar;

    public function __toString()
    {
        return '__toString';
    }
}

?>
--EXPECTF--
Fatal error: Enum may not include __toString in %s on line %d
