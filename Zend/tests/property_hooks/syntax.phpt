--TEST--
Basic property hook syntax
--FILE--
<?php

class Test {
    public $prop {
        get { }
        set { }
    }
}

?>
--EXPECT--
