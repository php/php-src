--TEST--
Cannot mix implicit and explicit get/set accessors
--FILE--
<?php

class Test {
    public $prop {
        get;
        set { echo "Set\n"; }
    }
}

?>
--EXPECTF--
Fatal error: Cannot specify both implicit and explicit accessors for the same property in %s on line %d
