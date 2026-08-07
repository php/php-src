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
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->setStaticPropertyValue());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->setStaticPropertyValue(null));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->setStaticPropertyValue(null,null));
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->setStaticPropertyValue(1.5, 'def'));
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->setStaticPropertyValue(array(1,2,3), 'blah'));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}


?>
--EXPECTF--
ArgumentCountError: ReflectionClass::setStaticPropertyValue() expects exactly 2 arguments, 3 given
ArgumentCountError: ReflectionClass::setStaticPropertyValue() expects exactly 2 arguments, 0 given
ArgumentCountError: ReflectionClass::setStaticPropertyValue() expects exactly 2 arguments, 1 given

Deprecated: ReflectionClass::setStaticPropertyValue(): Passing null to parameter #1 ($name) of type string is deprecated in %s on line %d
ReflectionException: Property C::$ does not exist
ReflectionException: Property C::$1.5 does not exist
TypeError: ReflectionClass::setStaticPropertyValue(): Argument #1 ($name) must be of type string, array given
