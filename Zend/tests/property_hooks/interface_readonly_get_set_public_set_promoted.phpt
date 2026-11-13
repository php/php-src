--TEST--
Readonly promoted public(set) property cannot implement set hook of get/set interface property
--FILE--
<?php
interface I {
    public int $prop { get; set; }
}
class C implements I {
    public function __construct(public public(set) readonly int $prop) {}
}
?>
--EXPECTF--
Fatal error: Readonly property C::$prop cannot implement set hook required by interface I::$prop in %s on line %d
