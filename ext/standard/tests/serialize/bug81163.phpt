--TEST--
Test __sleep returns non-array
--FILE--
<?php
class foo
{
    private $private = 'private';
}

class bar extends foo
{
    public function __sleep()
    {
        return (new bar());
    }
}

serialize(new bar());
?>
--EXPECTF--
Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Warning: serialize(): bar::__sleep() should return an array only containing the names of instance-variables to serialize in %s on line %d
