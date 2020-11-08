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
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getProperty("a", "a"));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getProperty(null));
} catch (exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getProperty(1));
} catch (exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getProperty(1.5));
} catch (exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getProperty(true));
} catch (exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getProperty(array(1,2,3)));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getProperty(new C));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Check invalid params:
ReflectionClass::getProperty() expects exactly 1 argument, 0 given
ReflectionClass::getProperty() expects exactly 1 argument, 2 given
Property C::$ does not exist
Property C::$1 does not exist
Property C::$1.5 does not exist
Property C::$1 does not exist
ReflectionClass::getProperty(): Argument #1 ($name) must be of type string, array given
ReflectionClass::getProperty(): Argument #1 ($name) must be of type string, C given
