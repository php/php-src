--TEST--
Constructor promotion with null default, requires an explicitly nullable type
--FILE--
<?php

class Test {
    public function __construct(public int $x = null) {}
}

?>
--EXPECTF--
Fatal error: Test::__construct(): Parameter #1 ($x) of type int cannot have a default value of type null in %s on line %d
