--TEST--
Constructor promotion can only be used in constructors ... duh
--FILE--
<?php

class Test {
    public function foobar(public int $x, public int $y) {}
}

?>
--EXPECTF--
Fatal error: Test::foobar(): Promoted property $x cannot be declared outside a constructor in %s on line %d
