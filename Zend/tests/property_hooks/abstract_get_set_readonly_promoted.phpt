--TEST--
Readonly promoted property cannot implement set hook of get/set abstract property
--FILE--
<?php
abstract class A {
    protected abstract int $prop { get; set; }
}
class C extends A {
    public function __construct(protected readonly int $prop) {}
}
?>
--EXPECTF--
Fatal error: Readonly property C::$prop cannot implement set hook required by class A::$prop in %s on line %d
