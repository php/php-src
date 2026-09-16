--TEST--
ReflectionProperty::__toString() - readonly with default
--FILE--
<?php

class Test {
    public readonly int $nick = 42;
}

echo new ReflectionProperty(Test::class, 'nick');

?>
--EXPECT--
Property [ public protected(set) readonly int $nick = 42 ]
