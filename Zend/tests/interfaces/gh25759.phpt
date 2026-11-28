--TEST--
GH-25759: Interface method compatibility is independent of extends order
--FILE--
<?php
interface HasOptionalValue
{
    public function getValue(): ?int;
}

interface HasValue
{
    public function getValue(): int;
}

interface OptionalValueFirst extends HasOptionalValue, HasValue
{
}

interface ValueFirst extends HasValue, HasOptionalValue
{
}

class OptionalValueFirstImplementation implements OptionalValueFirst
{
    public function getValue(): int
    {
        return 42;
    }
}

class ValueFirstImplementation implements ValueFirst
{
    public function getValue(): int
    {
        return 42;
    }
}

var_dump((new OptionalValueFirstImplementation())->getValue());
var_dump((new ValueFirstImplementation())->getValue());
?>
--EXPECT--
int(42)
int(42)
