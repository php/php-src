--TEST--
Trait property hook conflict
--FILE--
<?php

trait T {
    public $prop {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }
}

class C {
    use T;

    public $prop {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }
}

?>
--EXPECTF--
Fatal error: C and T define the same hooked property ($prop) in the composition of C. Conflict resolution between hooked properties is currently not supported. Class was composed in %s on line %d
