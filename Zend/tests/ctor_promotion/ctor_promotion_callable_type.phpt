--TEST--
Type of promoted property may not be callable
--FILE--
<?php

class Test {
    public function __construct(public callable $callable) {}
}

?>
--EXPECTF--
Fatal error: Property Test::$callable cannot have type callable in %s on line %d
