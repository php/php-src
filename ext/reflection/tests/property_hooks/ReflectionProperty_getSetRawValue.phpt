--TEST--
ReflectionProperty::{get,set}RawValue() does not invoke hooks
--FILE--
<?php

class Test {
    public $publicProp {
        get {
            echo __FUNCTION__, "\n";
            return $this->publicProp;
        }
        set {
            echo __FUNCTION__, "\n";
            $field = $value;
        }
    }
    public $privateProp {
        get {
            echo __FUNCTION__, "\n";
            return $this->privateProp;
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
    private $plainProp;

    public function __get($name) {
        echo __FUNCTION__, "\n";
        return 42;
    }
}

class Unguarded {
    private $plainProp;
}

function test($class, $prop) {
    $propertyReflection = (new ReflectionProperty($class, $prop));
    $object = new $class();
    try {
        $propertyReflection->setRawValue($object, 42);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        var_dump($propertyReflection->getRawValue($object));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

test(Test::class, 'publicProp');
test(Test::class, 'privateProp');
test(Test::class, 'virtualProp');
test(Test::class, 'plainProp');
test(Unguarded::class, 'plainProp');

?>
--EXPECT--
int(42)
int(42)
Must not write to virtual property Test::$virtualProp
Must not read from virtual property Test::$virtualProp
int(42)
int(42)
