--TEST--
GH-15140: Asymmetric set type cannot be satisfied by plain property
--FILE--
<?php

interface I {
    public string $prop {
        set(int|string $value);
    }
}
class C implements I {
    public string $prop;
}

?>
--EXPECTF--
Fatal error: Set type of C::$prop must be supertype of string|int (as in interface I) in %s on line %d
