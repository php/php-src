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
    var_dump($rc->getProperty(''));
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
?>
--EXPECTF--
Check invalid params:
Property C::$ does not exist
Property C::$1 does not exist
Property C::$1.5 does not exist
