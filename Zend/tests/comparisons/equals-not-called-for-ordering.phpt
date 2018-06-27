--TEST--
__equals: __equals should not be called when comparing for ordering
--FILE--
<?php
class Equatable
{
    public function __equals($other)
    {
        throw new Exception();
    }
}

new Equatable < new Equatable;
new Equatable > new Equatable;
new Equatable >= new Equatable;
new Equatable <= new Equatable;
new Equatable <=> new Equatable;
?>
--EXPECT--
