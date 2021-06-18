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
Notice: serialize(): "private" returned as member variable from __sleep() but does not exist in %s on line %d

