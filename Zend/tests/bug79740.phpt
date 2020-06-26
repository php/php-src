--TEST--
Bug #79740: serialize() and unserialize() methods can not be called statically
--FILE--
<?php
class A {
    public function serialize() { }
    public function unserialize() { }
}

var_dump(is_callable(['A', 'serialize']));
var_dump(is_callable(['A', 'unserialize']));
A::serialize();
A::unserialize();

?>
--EXPECTF--
bool(true)
bool(true)

Deprecated: Non-static method A::serialize() should not be called statically in %s on line %d

Deprecated: Non-static method A::unserialize() should not be called statically in %s on line %d
