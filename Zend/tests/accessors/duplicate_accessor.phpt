--TEST--
Cannot declare same accessor twice
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
Fatal error: Cannot redeclare accessor "get" in %s on line %d
