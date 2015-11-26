--TEST--
Test ReflectionProperty::export() usage.
--FILE--
<?php

class TestClass {
    public $proper = 5;
}

var_dump(ReflectionProperty::export('TestClass', 'proper'));

?>
--EXPECT--
Property [ <default> public $proper ]

NULL
