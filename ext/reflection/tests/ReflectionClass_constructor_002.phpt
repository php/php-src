--TEST--
ReflectionClass::__constructor() - bad arguments
--FILE--
<?php
try {
    var_dump(new ReflectionClass());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(new ReflectionClass(null));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(new ReflectionClass(true));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(new ReflectionClass(1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(new ReflectionClass(array(1,2,3)));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(new ReflectionClass("stdClass", 1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(new ReflectionClass("X"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
ArgumentCountError: ReflectionClass::__construct() expects exactly 1 argument, 0 given

Deprecated: ReflectionClass::__construct(): Passing null to parameter #1 ($objectOrClass) of type object|string is deprecated in %s on line %d
ReflectionException: Class "" does not exist
ReflectionException: Class "1" does not exist
ReflectionException: Class "1" does not exist
TypeError: ReflectionClass::__construct(): Argument #1 ($objectOrClass) must be of type object|string, array given
ArgumentCountError: ReflectionClass::__construct() expects exactly 1 argument, 2 given
ReflectionException: Class "X" does not exist
