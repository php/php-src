--TEST--
ReflectionClass::setStaticPropertyValue() - bad params
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
    public static $x;
}

$rc = new ReflectionClass('C');
try {
    var_dump($rc->setStaticPropertyValue("x", "default value", 'blah'));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->setStaticPropertyValue());
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->setStaticPropertyValue(null));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->setStaticPropertyValue(null,null));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->setStaticPropertyValue(1.5, 'def'));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->setStaticPropertyValue(array(1,2,3), 'blah'));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}


?>
--EXPECT--
ReflectionClass::setStaticPropertyValue() expects exactly 2 arguments, 3 given
ReflectionClass::setStaticPropertyValue() expects exactly 2 arguments, 0 given
ReflectionClass::setStaticPropertyValue() expects exactly 2 arguments, 1 given
Class C does not have a property named 
Class C does not have a property named 1.5
ReflectionClass::setStaticPropertyValue(): Argument #1 ($name) must be of type string, array given
