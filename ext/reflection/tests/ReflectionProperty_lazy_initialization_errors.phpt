--TEST--
Test ReflectionProperty::setRawValueWithoutLazyInitialization() and skipLazyInitialization() errors
--FILE--
<?php

#[AllowDynamicProperties]
class Demo {
    public static $myStatic;

    public bool $myVirtual {
        get => true;
    }
}

function test(object $obj, string $propertyName) {
    $r = new ReflectionProperty($obj, $propertyName);
    try {
        $r->setRawValueWithoutLazyInitialization($obj, true);
    } catch (ReflectionException $e) {
        echo $e->getMessage() . "\n";
    }
    try {
        $r->skipLazyInitialization($obj);
    } catch (ReflectionException $e) {
        echo $e->getMessage() . "\n\n";
    }
}

$obj = new Demo();
test($obj, 'myStatic');
test($obj, 'myVirtual');

$obj->myDynamic = true;
test($obj, 'myDynamic');

$obj = new ReflectionClass(Demo::class);
test($obj, 'name');

?>
--EXPECT--
Can not use setRawValueWithoutLazyInitialization on static property Demo::$myStatic
Can not use skipLazyInitialization on static property Demo::$myStatic

Can not use setRawValueWithoutLazyInitialization on virtual property Demo::$myVirtual
Can not use skipLazyInitialization on virtual property Demo::$myVirtual

Can not use setRawValueWithoutLazyInitialization on dynamic property Demo::$myDynamic
Can not use skipLazyInitialization on dynamic property Demo::$myDynamic

Can not use setRawValueWithoutLazyInitialization on internal class ReflectionClass
Can not use skipLazyInitialization on internal class ReflectionClass
