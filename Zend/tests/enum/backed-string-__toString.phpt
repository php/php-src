--TEST--
Backed string enum __toString
--FILE--
<?php

enum Foo: string {
    case Bar = 'bar';

    public function __toString()
    {
        return '__toString';
    }
}

?>
--EXPECTF--
Fatal error: Cannot redeclare Foo::__toString() in %s on line %d
