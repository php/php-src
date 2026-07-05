--TEST--
Readonly public(set) property does not implement set hook of get/set interface property
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
Fatal error: Class C contains 1 abstract method and must therefore be declared abstract or implement the remaining method (I::$prop::set) in %s on line %d
