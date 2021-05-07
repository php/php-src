--TEST--
Cannot have only &get (implicit)
--FILE--
<?php

class Test {
    public $prop { &get; }
}

?>
--EXPECTF--
Fatal error: Cannot have &get without set. Either remove the "&" or add "set" accessor in %s on line %d
