--TEST--
Public readonly property has restricted set visibility for public get/set abstract property
--FILE--
<?php
abstract class P {
    public abstract int $prop { get; set; }
}
class C extends P {
    public function __construct(public readonly int $prop) {}
}
?>
--EXPECTF--
Fatal error: Set access level of readonly property C::$prop is protected(set), but must be public(set) (as in class P) in %s on line %d
