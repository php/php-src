--TEST--
Readonly property with default value has restricted set visibility for get/set interface property
--DESCRIPTION--
The error message should be improved, the set access level comes from readonly. Ref: Zend/tests/property_hooks/interface_get_set_readonly.phpt
--FILE--
<?php

interface I {
    public int $prop { get; set; }
}

class C implements I {
    public readonly int $prop = 42;
}
?>
--EXPECTF--
Fatal error: Set access level of C::$prop must be omitted (as in class I) in %s on line %d
