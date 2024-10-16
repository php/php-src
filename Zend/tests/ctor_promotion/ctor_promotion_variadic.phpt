--TEST--
Cannot use constructor promotion with variadic parameter
--FILE--
<?php

class Test {
    public function __construct(public string ...$strings) {}
}

?>
--EXPECTF--
Fatal error: Cannot declare variadic promoted property in %s on line %d
