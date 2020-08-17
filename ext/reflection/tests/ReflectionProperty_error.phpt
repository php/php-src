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
    echo "Ok - ".$re->getMessage().PHP_EOL;
}
try {
    new ReflectionProperty('C::p');
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}

try {
    new ReflectionProperty('C', 'p', 'x');
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}

?>
--EXPECT--
Ok - ReflectionProperty::__construct(): Exactly 2 arguments are expected, 0 given
Ok - ReflectionProperty::__construct(): Exactly 2 arguments are expected, 1 given
Ok - ReflectionProperty::__construct(): Exactly 2 arguments are expected, 3 given
