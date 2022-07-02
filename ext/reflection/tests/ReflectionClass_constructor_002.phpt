--TEST--
ReflectionClass::__constructor() - bad arguments
--FILE--
<?php
try {
    var_dump(new ReflectionClass());
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(new ReflectionClass(null));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(new ReflectionClass(true));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(new ReflectionClass(1));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(new ReflectionClass(array(1,2,3)));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(new ReflectionClass("stdClass", 1));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(new ReflectionClass("X"));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
ReflectionClass::__construct() expects exactly 1 argument, 0 given

Deprecated: ReflectionClass::__construct(): Passing null to parameter #1 ($objectOrClass) of type object|string is deprecated in %s on line %d
Class "" does not exist
Class "1" does not exist
Class "1" does not exist
ReflectionClass::__construct(): Argument #1 ($objectOrClass) must be of type object|string, array given
ReflectionClass::__construct() expects exactly 1 argument, 2 given
Class "X" does not exist
