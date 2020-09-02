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
Notice: serialize(): A::__sleep should return an array only containing the names of instance-variables to serialize in %sbug79526.php on line %d

Notice: serialize(): B::__sleep should return an array only containing the names of instance-variables to serialize in %sbug79526.php on line %d

Notice: serialize(): "1" returned as member variable from __sleep() but does not exist in %sbug79526.php on line %d
Done
