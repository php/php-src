--TEST--
Test "or null"/"or be null" in type-checking errors for userland functions
--FILE--
<?php

// This should test every branch in zend_execute.c's `zend_verify_arg_type`, `zend_verify_return_type` and `zend_verify_missing_return_type` functions which produces an "or null"/"or be null" part in its error message

function unloadedClass(?I\Dont\Exist $param) {}

try {
    unloadedClass(new \StdClass);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

class RealClass {}
interface RealInterface {}

function loadedClass(?RealClass $param) {}
function loadedInterface(?RealInterface $param) {}

try {
    loadedClass(new \StdClass);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

try {
    loadedInterface(new \StdClass);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

try {
    unloadedClass(1);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

try {
    loadedClass(1);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

try {
    loadedInterface(1);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function callableF(?callable $param) {}

try {
    callableF(1);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function iterableF(?iterable $param) {}

try {
    iterableF(1);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function intF(?int $param) {}

try {
    intF(new StdClass);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnUnloadedClass(): ?I\Dont\Exist {
    return new \StdClass;
}

try {
    returnUnloadedClass();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnLoadedClass(): ?RealClass {
    return new \StdClass;
}

try {
    returnLoadedClass();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnLoadedInterface(): ?RealInterface {
    return new \StdClass;
}

try {
    returnLoadedInterface();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnUnloadedClassScalar(): ?I\Dont\Exist {
    return 1;
}

try {
    returnUnloadedClassScalar();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnLoadedClassScalar(): ?RealClass {
    return 1;
}

try {
    returnLoadedClassScalar();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnLoadedInterfaceScalar(): ?RealInterface {
    return 1;
}

try {
    returnLoadedInterfaceScalar();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnCallable(): ?callable {
    return 1;
}

try {
    returnCallable();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnIterable(): ?iterable {
    return 1;
}

try {
    returnIterable();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnInt(): ?int {
    return new \StdClass;
}

try {
    returnInt();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnMissingUnloadedClass(): ?I\Dont\Exist {
}

try {
    returnMissingUnloadedClass();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnMissingLoadedClass(): ?RealClass {
}

try {
    returnMissingLoadedClass();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnMissingLoadedInterface(): ?RealInterface {
}

try {
    returnMissingLoadedInterface();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnMissingCallable(): ?callable {
}

try {
    returnMissingCallable();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnMissingIterable(): ?iterable {
}

try {
    returnMissingIterable();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnMissingInt(): ?int {
}

try {
    returnMissingInt();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

?>
--EXPECTF--
TypeError: Argument 1 passed to unloadedClass() must be an instance of I\Dont\Exist or null, instance of stdClass given, called in %s/or_null.php on line 8 and defined in %s/or_null.php:5
Stack trace:
#0 %s/or_null.php(8): unloadedClass(Object(stdClass))
#1 {main}
TypeError: Argument 1 passed to loadedClass() must be an instance of RealClass or null, instance of stdClass given, called in %s/or_null.php on line 20 and defined in %s/or_null.php:16
Stack trace:
#0 %s/or_null.php(20): loadedClass(Object(stdClass))
#1 {main}
TypeError: Argument 1 passed to loadedInterface() must implement interface RealInterface or be null, instance of stdClass given, called in %s/or_null.php on line 26 and defined in %s/or_null.php:17
Stack trace:
#0 %s/or_null.php(26): loadedInterface(Object(stdClass))
#1 {main}
TypeError: Argument 1 passed to unloadedClass() must be an instance of I\Dont\Exist or null, integer given, called in %s/or_null.php on line 32 and defined in %s/or_null.php:5
Stack trace:
#0 %s/or_null.php(32): unloadedClass(1)
#1 {main}
TypeError: Argument 1 passed to loadedClass() must be an instance of RealClass or null, integer given, called in %s/or_null.php on line 38 and defined in %s/or_null.php:16
Stack trace:
#0 %s/or_null.php(38): loadedClass(1)
#1 {main}
TypeError: Argument 1 passed to loadedInterface() must implement interface RealInterface or be null, integer given, called in %s/or_null.php on line 44 and defined in %s/or_null.php:17
Stack trace:
#0 %s/or_null.php(44): loadedInterface(1)
#1 {main}
TypeError: Argument 1 passed to callableF() must be callable or null, integer given, called in %s/or_null.php on line 52 and defined in %s/or_null.php:49
Stack trace:
#0 %s/or_null.php(52): callableF(1)
#1 {main}
TypeError: Argument 1 passed to iterableF() must be iterable or null, integer given, called in %s/or_null.php on line 60 and defined in %s/or_null.php:57
Stack trace:
#0 %s/or_null.php(60): iterableF(1)
#1 {main}
TypeError: Argument 1 passed to intF() must be of the type integer or null, object given, called in %s/or_null.php on line 68 and defined in %s/or_null.php:65
Stack trace:
#0 %s/or_null.php(68): intF(Object(stdClass))
#1 {main}
TypeError: Return value of returnUnloadedClass() must be an instance of I\Dont\Exist or null, instance of stdClass returned in %s/or_null.php:74
Stack trace:
#0 %s/or_null.php(78): returnUnloadedClass()
#1 {main}
TypeError: Return value of returnLoadedClass() must be an instance of RealClass or null, instance of stdClass returned in %s/or_null.php:84
Stack trace:
#0 %s/or_null.php(88): returnLoadedClass()
#1 {main}
TypeError: Return value of returnLoadedInterface() must implement interface RealInterface or be null, instance of stdClass returned in %s/or_null.php:94
Stack trace:
#0 %s/or_null.php(98): returnLoadedInterface()
#1 {main}
TypeError: Return value of returnUnloadedClassScalar() must be an instance of I\Dont\Exist or null, integer returned in %s/or_null.php:104
Stack trace:
#0 %s/or_null.php(108): returnUnloadedClassScalar()
#1 {main}
TypeError: Return value of returnLoadedClassScalar() must be an instance of RealClass or null, integer returned in %s/or_null.php:114
Stack trace:
#0 %s/or_null.php(118): returnLoadedClassScalar()
#1 {main}
TypeError: Return value of returnLoadedInterfaceScalar() must implement interface RealInterface or be null, integer returned in %s/or_null.php:124
Stack trace:
#0 %s/or_null.php(128): returnLoadedInterfaceScalar()
#1 {main}
TypeError: Return value of returnCallable() must be callable or null, integer returned in %s/or_null.php:134
Stack trace:
#0 %s/or_null.php(138): returnCallable()
#1 {main}
TypeError: Return value of returnIterable() must be iterable or null, integer returned in %s/or_null.php:144
Stack trace:
#0 %s/or_null.php(148): returnIterable()
#1 {main}
TypeError: Return value of returnInt() must be of the type integer or null, object returned in %s/or_null.php:154
Stack trace:
#0 %s/or_null.php(158): returnInt()
#1 {main}
TypeError: Return value of returnMissingUnloadedClass() must be an instance of I\Dont\Exist or null, none returned in %s/or_null.php:164
Stack trace:
#0 %s/or_null.php(167): returnMissingUnloadedClass()
#1 {main}
TypeError: Return value of returnMissingLoadedClass() must be an instance of RealClass or null, none returned in %s/or_null.php:173
Stack trace:
#0 %s/or_null.php(176): returnMissingLoadedClass()
#1 {main}
TypeError: Return value of returnMissingLoadedInterface() must implement interface RealInterface or be null, none returned in %s/or_null.php:182
Stack trace:
#0 %s/or_null.php(185): returnMissingLoadedInterface()
#1 {main}
TypeError: Return value of returnMissingCallable() must be callable or null, none returned in %s/or_null.php:191
Stack trace:
#0 %s/or_null.php(194): returnMissingCallable()
#1 {main}
TypeError: Return value of returnMissingIterable() must be iterable or null, none returned in %s/or_null.php:200
Stack trace:
#0 %s/or_null.php(203): returnMissingIterable()
#1 {main}
TypeError: Return value of returnMissingInt() must be of the type integer or null, none returned in %s/or_null.php:209
Stack trace:
#0 %s/or_null.php(212): returnMissingInt()
#1 {main}
