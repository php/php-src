--TEST--
get accessor may not have parameters
--FILE--
<?php

class Test {
    public $prop {
        get($param) {}
    }
}

?>
--EXPECTF--
Fatal error: Accessor "get" may not have parameters in %s on line %d
