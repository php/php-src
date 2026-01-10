--TEST--
Constructor promotion with null default, requires an explicitly nullable type
--FILE--
<?php

class Test {
    public function __construct(public int $x = null) {}
}

?>
--EXPECTF--
Fatal error: Cannot use null as default value for parameter $x of type int in %s on line %d
