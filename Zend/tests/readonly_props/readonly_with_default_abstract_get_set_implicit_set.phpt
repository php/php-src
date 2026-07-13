--TEST--
Readonly property with default value has restricted set visibility for get/set abstract property
--DESCRIPTION--
The error message should be improved, the set access level comes from readonly.
--FILE--
<?php

abstract class P {
    public abstract int $prop { get; set; }
}

class C extends P {
    public readonly int $prop = 42;
}

?>
--EXPECTF--
Fatal error: Set access level of C::$prop must be omitted (as in class P) in %s on line %d
