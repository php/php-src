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
TypeError: unloadedClass(): Argument #1 ($param) must be of type ?I\Dont\Exist, stdClass given, called in %s:%d
Stack trace:
#0 %s(8): unloadedClass(Object(stdClass))
#1 {main}
TypeError: loadedClass(): Argument #1 ($param) must be of type ?RealClass, stdClass given, called in %s:%d
Stack trace:
#0 %s(20): loadedClass(Object(stdClass))
#1 {main}
TypeError: loadedInterface(): Argument #1 ($param) must be of type ?RealInterface, stdClass given, called in %s:%d
Stack trace:
#0 %s(26): loadedInterface(Object(stdClass))
#1 {main}
TypeError: unloadedClass(): Argument #1 ($param) must be of type ?I\Dont\Exist, int given, called in %s:%d
Stack trace:
#0 %s(32): unloadedClass(1)
#1 {main}
TypeError: loadedClass(): Argument #1 ($param) must be of type ?RealClass, int given, called in %s:%d
Stack trace:
#0 %s(38): loadedClass(1)
#1 {main}
TypeError: loadedInterface(): Argument #1 ($param) must be of type ?RealInterface, int given, called in %s:%d
Stack trace:
#0 %s(44): loadedInterface(1)
#1 {main}
TypeError: callableF(): Argument #1 ($param) must be of type ?callable, int given, called in %s:%d
Stack trace:
#0 %s(52): callableF(1)
#1 {main}
TypeError: iterableF(): Argument #1 ($param) must be of type ?iterable, int given, called in %s:%d
Stack trace:
#0 %s(60): iterableF(1)
#1 {main}
TypeError: intF(): Argument #1 ($param) must be of type ?int, stdClass given, called in %s:%d
Stack trace:
#0 %s(68): intF(Object(stdClass))
#1 {main}
TypeError: returnUnloadedClass(): Return value must be of type ?I\Dont\Exist, stdClass returned in %s:%d
Stack trace:
#0 %s(78): returnUnloadedClass()
#1 {main}
TypeError: returnLoadedClass(): Return value must be of type ?RealClass, stdClass returned in %s:%d
Stack trace:
#0 %s(88): returnLoadedClass()
#1 {main}
TypeError: returnLoadedInterface(): Return value must be of type ?RealInterface, stdClass returned in %s:%d
Stack trace:
#0 %s(98): returnLoadedInterface()
#1 {main}
TypeError: returnUnloadedClassScalar(): Return value must be of type ?I\Dont\Exist, int returned in %s:%d
Stack trace:
#0 %s(108): returnUnloadedClassScalar()
#1 {main}
TypeError: returnLoadedClassScalar(): Return value must be of type ?RealClass, int returned in %s:%d
Stack trace:
#0 %s(118): returnLoadedClassScalar()
#1 {main}
TypeError: returnLoadedInterfaceScalar(): Return value must be of type ?RealInterface, int returned in %s:%d
Stack trace:
#0 %s(128): returnLoadedInterfaceScalar()
#1 {main}
TypeError: returnCallable(): Return value must be of type ?callable, int returned in %s:%d
Stack trace:
#0 %s(138): returnCallable()
#1 {main}
TypeError: returnIterable(): Return value must be of type ?iterable, int returned in %s:%d
Stack trace:
#0 %s(148): returnIterable()
#1 {main}
TypeError: returnInt(): Return value must be of type ?int, stdClass returned in %s:%d
Stack trace:
#0 %s(158): returnInt()
#1 {main}
TypeError: returnMissingUnloadedClass(): Return value must be of type ?I\Dont\Exist, none returned in %s:%d
Stack trace:
#0 %s(167): returnMissingUnloadedClass()
#1 {main}
TypeError: returnMissingLoadedClass(): Return value must be of type ?RealClass, none returned in %s:%d
Stack trace:
#0 %s(176): returnMissingLoadedClass()
#1 {main}
TypeError: returnMissingLoadedInterface(): Return value must be of type ?RealInterface, none returned in %s:%d
Stack trace:
#0 %s(185): returnMissingLoadedInterface()
#1 {main}
TypeError: returnMissingCallable(): Return value must be of type ?callable, none returned in %s:%d
Stack trace:
#0 %s(194): returnMissingCallable()
#1 {main}
TypeError: returnMissingIterable(): Return value must be of type ?iterable, none returned in %s:%d
Stack trace:
#0 %s(203): returnMissingIterable()
#1 {main}
TypeError: returnMissingInt(): Return value must be of type ?int, none returned in %s:%d
Stack trace:
#0 %s(212): returnMissingInt()
#1 {main}
