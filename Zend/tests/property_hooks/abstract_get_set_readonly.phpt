--TEST--
Readonly property cannot implement set hook of get/set abstract property
--FILE--
<?php
abstract class A {
    protected abstract int $prop { get; set; }
}
class C extends A {
    protected readonly int $prop;
    public function __construct(int $prop) {
        $this->prop = $prop;
    }
}
?>
--EXPECTF--
Fatal error: Readonly property C::$prop cannot implement set hook required by class A::$prop in %s on line %d
