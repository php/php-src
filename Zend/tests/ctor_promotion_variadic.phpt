--TEST--
Cannot use constructor promotion with variadic parameter
--FILE--
<?php

class Test {
    public function __construct(public string ...$strings) {}
}

?>
--EXPECTF--
Fatal error: Variadic promoted property Test::$strings cannot be declared in %s on line %d
