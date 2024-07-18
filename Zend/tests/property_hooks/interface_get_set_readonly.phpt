--TEST--
readonly property does not satisfy get/set interface property
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
Fatal error: Readonly property C::$prop does not satisfy abstract read-write property I::$prop in %s on line %d
