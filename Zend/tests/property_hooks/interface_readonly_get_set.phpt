--TEST--
Readonly property cannot implement set hook of get/set interface property
--FILE--
<?php
interface I {
    public int $prop { get; set; }
}
class C implements I {
    public readonly int $prop;
    public function __construct(int $prop) {
        $this->prop = $prop;
    }
}
?>
--EXPECTF--
Fatal error: Readonly property C::$prop cannot implement set hook required by interface I::$prop in %s on line %d
