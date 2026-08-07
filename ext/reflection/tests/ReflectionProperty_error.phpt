--TEST--
Test ReflectionProperty class errors.
--FILE--
<?php

class C {
    public static $p;
}

try {
    new ReflectionProperty();
} catch (TypeError $re) {
    echo $re::class, ': ', $re->getMessage(), PHP_EOL;
}
try {
    new ReflectionProperty('C::p');
} catch (TypeError $re) {
    echo $re::class, ': ', $re->getMessage(), PHP_EOL;
}

try {
    new ReflectionProperty('C', 'p', 'x');
} catch (TypeError $re) {
    echo $re::class, ': ', $re->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ArgumentCountError: ReflectionProperty::__construct() expects exactly 2 arguments, 0 given
ArgumentCountError: ReflectionProperty::__construct() expects exactly 2 arguments, 1 given
ArgumentCountError: ReflectionProperty::__construct() expects exactly 2 arguments, 3 given
