--TEST--
Bug #79526 (`__sleep` error message doesn't include the name of the class)
--FILE--
<?php
class A
{
    public function __sleep() {
        return 1;
    }
}


serialize(new A());

class B
{
    public function __sleep() {
        return [1];
    }
}


serialize(new B());
?>
Done
--EXPECTF--
Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Warning: serialize(): A::__sleep() should return an array only containing the names of instance-variables to serialize in %s on line %d

Warning: serialize(): B::__sleep() should return an array only containing the names of instance-variables to serialize in %s on line %d

Warning: serialize(): "1" returned as member variable from __sleep() but does not exist in %s on line %d
Done
