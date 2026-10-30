--TEST--
Readonly promoted property cannot implement set hook of get/set interface property
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
Fatal error: Readonly property C::$prop cannot implement set hook required by interface I::$prop in %s on line %d
