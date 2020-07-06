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
Fatal error: Promoted property Test::$prop cannot redeclare standard property in %s on line %d
