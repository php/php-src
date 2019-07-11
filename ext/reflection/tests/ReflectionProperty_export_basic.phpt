--TEST--
Test ReflectionProperty::__toString() usage.
--FILE--
<?php

class TestClass {
    public $proper = 5;
}

echo new ReflectionProperty('TestClass', 'proper');

?>
--EXPECT--
Property [ <default> public $proper ]
