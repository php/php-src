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
    var_dump($rc->getMethod());
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getMethod("f", "f"));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getMethod(null));
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
try {
    var_dump($rc->getMethod(true));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getMethod(array(1,2,3)));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->getMethod(new C));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}


?>
--EXPECTF--
Check invalid params:
ReflectionClass::getMethod() expects exactly 1 argument, 0 given
ReflectionClass::getMethod() expects exactly 1 argument, 2 given

Deprecated: ReflectionClass::getMethod(): Passing null to parameter #1 ($name) of type string is deprecated in %s on line %d
Method C::() does not exist
Method C::1() does not exist
Method C::1.5() does not exist
Method C::1() does not exist
ReflectionClass::getMethod(): Argument #1 ($name) must be of type string, array given
ReflectionClass::getMethod(): Argument #1 ($name) must be of type string, C given
