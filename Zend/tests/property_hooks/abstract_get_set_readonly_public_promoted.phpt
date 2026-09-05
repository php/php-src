--TEST--
Readonly promoted property cannot implement set hook of public get/set abstract property
--FILE--
<?php
abstract class A {
    public abstract int $prop { get; set; }
}
class C extends A {
    public function __construct(public readonly int $prop) {}
}
?>
--EXPECTF--
Fatal error: Readonly property C::$prop cannot implement set hook required by class A::$prop in %s on line %d
