--TEST--
ReflectionProperty::{get,set}Value() invokes hooks
--FILE--
<?php

class Test {
    public $a {
        get {
            echo "Get called\n";
            return 'Foo';
        }
        set {
            echo "Set called with value $value\n";
        }
    }
}

$propertyReflection = (new ReflectionProperty(Test::class, 'a'));

$test = new Test();
var_dump($propertyReflection->getValue($test));
$propertyReflection->setValue($test, 'Baz');

?>
--EXPECT--
Get called
string(3) "Foo"
Set called with value Baz
