--TEST--
ZE2 Late Static Binding testing get_called_class()
--FILE--
<?php

class TestClass {
    public static function getClassName() {
        return get_called_class();
    }
}

class ChildClass extends TestClass {}

echo TestClass::getClassName() . "\n";
echo ChildClass::getClassName() . "\n";
?>
--EXPECT--
TestClass
ChildClass
