--TEST--
ReflectionClass::getMethod() - error cases
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
    function f() {}
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
try {
    var_dump($rc->getMethod(''));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getMethod(1));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getMethod(1.5));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Check invalid params:
Method C::() does not exist
Method C::1() does not exist
Method C::1.5() does not exist
