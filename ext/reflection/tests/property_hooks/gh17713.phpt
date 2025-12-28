--TEST--
GH-17713: getRawValue(), setRawValue(), setRawValueWithoutLazyInitialization() may call hooks
--FILE--
<?php

#[AllowDynamicProperties]
class Base {
    public $publicProp = 'Base::$publicProp';
    private $privateProp = 'Base::$privateProp';
    public $hookedProp {
        get {
            echo __METHOD__, "\n";
            return $this->hookedProp;
        }
        set {
            echo __METHOD__, "\n";
            $this->hookedProp = $value;
        }
    }
    public $virtualProp {
        get {
            echo __METHOD__, "\n";
            return __METHOD__;
        }
        set {
            echo __METHOD__, "\n";
        }
    }
}

class Test extends Base {
    public $publicProp {
        get {
            echo __FUNCTION__, "\n";
            return $this->publicProp;
        }
        set {
            echo __FUNCTION__, "\n";
            $this->publicProp = $value;
        }
    }
    private $privateProp = 'Test::$privateProp' {
        get {
            echo __FUNCTION__, "\n";
            return $this->privateProp;
        }
        set {
            echo __FUNCTION__, "\n";
            $this->privateProp = $value;
        }
    }
    public $dynamicProp {
        get {
            echo __FUNCTION__, "\n";
            return $this->dynamicProp;
        }
        set {
            echo __FUNCTION__, "\n";
            $this->dynamicProp = $value;
        }
    }
    public $hookedProp {
        get {
            echo __METHOD__, "\n";
            return $this->hookedProp;
        }
        set {
            echo __METHOD__, "\n";
            $this->hookedProp = $value;
        }
    }
    public $virtualProp {
        get {
            echo __METHOD__, "\n";
            return $this->virtualProp;
        }
        set {
            echo __METHOD__, "\n";
            $this->virtualProp = $value;
        }
    }
    public static $changedProp = 'Test::$changedProp';
}

function test($scope, $class, $prop) {
    printf(
        "# Accessing %s->%s from scope %s\n",
        $class,
        $prop,
        is_object($scope) ? get_class($scope) : $scope,
    );

    $propertyReflection = new ReflectionProperty($scope, $prop);
    $object = new $class();
    try {
        $propertyReflection->setRawValue($object, 42);
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        var_dump($propertyReflection->getRawValue($object));
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        $propertyReflection->setRawValueWithoutLazyInitialization($object, 43);
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        var_dump($propertyReflection->getRawValue($object));
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
    if ($prop === 'privateProp') {
        printf(
            "# Accessing %s->%s again from scope %s\n",
            $class,
            $prop,
            $class,
        );
        $propertyReflection = new ReflectionProperty($class, $prop);
        var_dump($propertyReflection->getRawValue($object));
    }
}

test(Base::class, Test::class, 'publicProp');
test(Base::class, Test::class, 'privateProp');
test(Base::class, Test::class, 'hookedProp');
test(Base::class, Test::class, 'virtualProp');
test(Base::class, Base::class, 'virtualProp');

$scope = new Base();
$scope->dynamicProp = 'Base::$dynamicProp';
$scope->changedProp = 'Base::$dynamicProp';

test($scope, Test::class, 'dynamicProp');
test($scope, Test::class, 'changedProp');

?>
--EXPECT--
# Accessing Test->publicProp from scope Base
int(42)
int(43)
# Accessing Test->privateProp from scope Base
int(42)
int(43)
# Accessing Test->privateProp again from scope Test
string(18) "Test::$privateProp"
# Accessing Test->hookedProp from scope Base
int(42)
int(43)
# Accessing Test->virtualProp from scope Base
int(42)
int(43)
# Accessing Base->virtualProp from scope Base
Must not write to virtual property Base::$virtualProp
Must not read from virtual property Base::$virtualProp
Can not use setRawValueWithoutLazyInitialization on virtual property Base::$virtualProp
Must not read from virtual property Base::$virtualProp
# Accessing Test->dynamicProp from scope Base
int(42)
int(43)
# Accessing Test->changedProp from scope Base
May not use setRawValue on static properties
May not use getRawValue on static properties
Can not use setRawValueWithoutLazyInitialization on static property Test::$changedProp
May not use getRawValue on static properties
