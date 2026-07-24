--TEST--
Readonly trait property default value mismatch
--FILE--
<?php

trait T1 {
    public readonly int $prop = 1;
}

trait T2 {
    public readonly int $prop = 2;
}

class C {
    use T1, T2;
}

?>
--EXPECTF--
Fatal error: T1 and T2 define the same property ($prop) in the composition of C. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
