--TEST--
Clash between promoted and explicit property
--FILE--
<?php

class Test {
    public $prop;

    public function __construct(public $prop) {}
}

?>
--EXPECTF--
Fatal error: Cannot redeclare Test::$prop in %s on line %d
