--TEST--
Constructor promotion cannot be used inside an abstract constructor
--FILE--
<?php

abstract class Test {
    abstract public function __construct(public int $x);
}

?>
--EXPECTF--
Fatal error: Promoted property Test::$x cannot be declared in an abstract constructor in %s on line %d
