--TEST--
Cannot declare same property hook twice
--FILE--
<?php

class Test {
    public $prop {
        get {}
        get {}
    }
}

?>
--EXPECTF--
Fatal error: Cannot redeclare property hook "get" in %s on line %d
