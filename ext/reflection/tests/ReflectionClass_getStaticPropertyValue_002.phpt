--TEST--
ReflectionClass::getStaticPropertyValue() - bad params
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
    var_dump($rc->getStaticPropertyValue("x", "default value", 'blah'));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getStaticPropertyValue());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getStaticPropertyValue(null));
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getStaticPropertyValue(1.5, 'def'));
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getStaticPropertyValue(array(1,2,3)));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}


?>
--EXPECTF--
ArgumentCountError: ReflectionClass::getStaticPropertyValue() expects at most 2 arguments, 3 given
ArgumentCountError: ReflectionClass::getStaticPropertyValue() expects at least 1 argument, 0 given

Deprecated: ReflectionClass::getStaticPropertyValue(): Passing null to parameter #1 ($name) of type string is deprecated in %s on line %d
ReflectionException: Property C::$ does not exist
string(3) "def"
TypeError: ReflectionClass::getStaticPropertyValue(): Argument #1 ($name) must be of type string, array given
