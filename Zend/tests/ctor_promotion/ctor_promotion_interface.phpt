--TEST--
Constructor promotion cannot be used inside an abstract constructor (interface variant)
--FILE--
<?php

interface Test {
    public function __construct(public int $x);
}

?>
--EXPECTF--
Fatal error: Cannot declare promoted property in an abstract constructor in %s on line %d
