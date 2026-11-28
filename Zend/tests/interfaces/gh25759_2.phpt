--TEST--
GH-25759: Interface method merging retains the most restrictive return type
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

interface Result extends HasOptionalValue, HasValue
{
}

class InvalidImplementation implements Result
{
    public function getValue(): ?int
    {
        return null;
    }
}
?>
--EXPECTF--
Fatal error: Declaration of InvalidImplementation::getValue(): ?int must be compatible with HasValue::getValue(): int in %s on line %d
