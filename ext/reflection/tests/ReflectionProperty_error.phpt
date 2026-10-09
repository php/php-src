--TEST--
Test ReflectionProperty class errors.
--FILE--
<?php

class C {
    public static $p;
}

try {
    new ReflectionProperty();
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}
try {
    new ReflectionProperty('C::p');
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}

try {
    new ReflectionProperty('C', 'p', 'x');
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: ReflectionProperty::__construct() expects exactly 2 arguments, 0 given
ArgumentCountError: ReflectionProperty::__construct() expects exactly 2 arguments, 1 given
ArgumentCountError: ReflectionProperty::__construct() expects exactly 2 arguments, 3 given
