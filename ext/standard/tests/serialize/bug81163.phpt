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
Warning: serialize(): bar::__sleep() should return an array of property names, or return null/void to delegate to default serialization in %s on line %d
