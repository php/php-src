--TEST--
Compatibility of trait properties with new default
--FILE--
<?php

class A {
    // Should not be called.
    public function __construct() {
        echo "Constructor\n";
    }
}

trait T1 {
    public $prop = new A;
}
trait T2 {
    public $prop = new A;
}

class B {
    use T1, T2;
}

?>
--EXPECTF--
Fatal error: T1 and T2 define the same property ($prop) in the composition of B. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
