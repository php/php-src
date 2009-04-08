--TEST--
Test ReflectionProperty::export() usage.
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
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
