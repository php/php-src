--TEST--
Cannot have only implicit &get
--FILE--
<?php

class Test {
    public $prop { &get; }
}

?>
--EXPECTF--
Fatal error: Cannot have implicit &get without set. Either remove the "&" or add "set" accessor in %s on line %d
