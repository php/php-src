--TEST--
readonly property does not satisfy get/set abstract property
--FILE--
<?php
abstract class P {
    protected abstract int $prop { get; set; }
}
class C extends P {
    public function __construct(protected readonly int $prop) {}
}
?>
--EXPECTF--
Fatal error: Class C contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (P::$prop::set) in %s on line %d
