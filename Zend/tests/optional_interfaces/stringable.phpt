--TEST--
Adding stringable is not broken with Optional interfaces
--FILE--
<?php

trait Str
{
    public function __toString() {}
}

class TestClass implements ?NonexistantInterface
{
    use Str;
}

echo implode(',', class_implements('TestClass'));

?>
--EXPECT--
Stringable
