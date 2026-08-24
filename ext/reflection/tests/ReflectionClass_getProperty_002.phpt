--TEST--
ReflectionClass::getProperty() - error cases
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
    public $a;
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
try {
    var_dump($rc->getProperty());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getProperty("a", "a"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getProperty(null));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getProperty(1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getProperty(1.5));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getProperty(true));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getProperty(array(1,2,3)));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($rc->getProperty(new C));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Check invalid params:
ArgumentCountError: ReflectionClass::getProperty() expects exactly 1 argument, 0 given
ArgumentCountError: ReflectionClass::getProperty() expects exactly 1 argument, 2 given

Deprecated: ReflectionClass::getProperty(): Passing null to parameter #1 ($name) of type string is deprecated in %s on line %d
ReflectionException: Property C::$ does not exist
ReflectionException: Property C::$1 does not exist
ReflectionException: Property C::$1.5 does not exist
ReflectionException: Property C::$1 does not exist
TypeError: ReflectionClass::getProperty(): Argument #1 ($name) must be of type string, array given
TypeError: ReflectionClass::getProperty(): Argument #1 ($name) must be of type string, C given
