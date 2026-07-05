--TEST--
Readonly property has restricted set visibility for get/set interface property
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
Fatal error: Set access level of readonly property C::$prop is protected(set), but must be public(set) (as in interface I) in %s on line %d
