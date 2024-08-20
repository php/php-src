--TEST--
Lazy objects: invalid options
--FILE--
<?php

class C {
    public $a = 1;
}

$reflector = new ReflectionClass(C::class);

try {
    $obj = $reflector->newLazyGhost(function ($obj) { }, -1);
} catch (ReflectionException $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

try {
    $obj = $reflector->newLazyProxy(function ($obj) { }, -1);
} catch (ReflectionException $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

try {
    // SKIP_DESTRUCTOR is only allowed on resetAsLazyProxy()
    $obj = $reflector->newLazyGhost(function ($obj) { }, ReflectionClass::SKIP_DESTRUCTOR);
} catch (ReflectionException $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$obj = new C();

try {
    $reflector->resetAsLazyGhost($obj, function ($obj) { }, -1);
} catch (ReflectionException $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

try {
    $reflector->resetAsLazyProxy($obj, function ($obj) { }, -1);
} catch (ReflectionException $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

?>
--EXPECT--
ReflectionException: ReflectionClass::newLazyGhost(): Argument #2 ($options) contains invalid flags
ReflectionException: ReflectionClass::newLazyProxy(): Argument #2 ($options) contains invalid flags
ReflectionException: ReflectionClass::newLazyGhost(): Argument #2 ($options) does not accept ReflectionClass::SKIP_DESTRUCTOR
ReflectionException: ReflectionClass::resetAsLazyGhost(): Argument #3 ($options) contains invalid flags
ReflectionException: ReflectionClass::resetAsLazyProxy(): Argument #3 ($options) contains invalid flags
