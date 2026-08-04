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
Cannot use setRawValueWithoutLazyInitialization() on static property Demo::$myStatic
Cannot use skipLazyInitialization() on static property Demo::$myStatic

Cannot use setRawValueWithoutLazyInitialization() on virtual property Demo::$myVirtual
Cannot use skipLazyInitialization() on virtual property Demo::$myVirtual

Cannot use setRawValueWithoutLazyInitialization() on dynamic property Demo::$myDynamic
Cannot use skipLazyInitialization() on dynamic property Demo::$myDynamic

Cannot use setRawValueWithoutLazyInitialization() on internal class ReflectionClass
Cannot use skipLazyInitialization() on internal class ReflectionClass
