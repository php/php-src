--TEST--
Type of promoted property may not be callable
--FILE--
<?php

class Test {
    public function __construct(public callable $callable) {}
}

?>
--EXPECTF--
Fatal error: Promoted property Test::$callable cannot be of type callable in %s on line %d
