--TEST--
readonly property does not satisfy get/set interface property
--DESCRIPTION--
The error message should be improved, the set access level comes from readonly.
--FILE--
<?php
interface I {
    public int $prop { get; set; }
}
class C implements I {
    public function __construct(public readonly int $prop) {}
}
?>
--EXPECTF--
Fatal error: Set access level of C::$prop must be omitted (as in class I) in %s on line %d
