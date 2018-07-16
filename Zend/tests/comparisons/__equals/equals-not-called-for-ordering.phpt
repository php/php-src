--TEST--
__equals: Should not be called when comparing for ordering
--FILE--
<?php
class A
{
    public function __equals($other)
    {
        echo __METHOD__;
    }
}

new A <   new A;
new A >   new A;
new A >=  new A;
new A <=  new A;
new A <=> new A;
?>
--EXPECT--
