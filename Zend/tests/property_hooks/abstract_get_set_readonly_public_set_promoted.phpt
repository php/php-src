--TEST--
Readonly promoted public(set) property cannot implement set hook of public get/set abstract property
--FILE--
<?php
abstract class A {
    public abstract int $prop { get; set; }
}
class C extends A {
    public function __construct(public public(set) readonly int $prop) {}
}
?>
--EXPECTF--
Fatal error: Readonly property C::$prop cannot implement set hook required by class A::$prop in %s on line %d
