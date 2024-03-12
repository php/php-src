--TEST--
ReflectionProperty::{get,set}RawValue() does not invoke hooks
--FILE--
<?php

class Test {
    public $publicProp {
        get {
            echo __FUNCTION__, "\n";
            return $field;
        }
        set {
            echo __FUNCTION__, "\n";
            $field = $value;
        }
    }
    public $privateProp {
        get {
            echo __FUNCTION__, "\n";
            return $field;
        }
        set {
            echo __FUNCTION__, "\n";
            $field = $value;
        }
    }
    public $virtualProp {
        get {
            echo __FUNCTION__, "\n";
            return 42;
        }
        set {
            echo __FUNCTION__, "\n";
        }
    }
}

function test($prop) {
    $propertyReflection = (new ReflectionProperty(Test::class, $prop));
    $test = new Test();
    try {
        $propertyReflection->setRawValue($test, 42);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        var_dump($propertyReflection->getRawValue($test));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

test('publicProp');
test('privateProp');
test('virtualProp');

?>
--EXPECT--
int(42)
int(42)
Must not write to virtual property Test::$virtualProp
Must not read from virtual property Test::$virtualProp
