--TEST--
Trait accessor conflict
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
Fatal error: C and T define the same accessor property ($prop) in the composition of C. Conflict resolution between accessor properties is currently not supported. Class was composed in %s on line %d
