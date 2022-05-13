--TEST--
Trying to acquire callable to something that's not callable
--FILE--
<?php

class Test {
    private static function privateMethod() {}

    public function instanceMethod() {}
}

try {
    $fn = 123;
    $fn(...);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    does_not_exist(...);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    stdClass::doesNotExist(...);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    (new stdClass)->doesNotExist(...);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    [new stdClass, 'doesNotExist'](...);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    Test::privateMethod(...);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    Test::instanceMethod(...);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Value of type int is not callable
Call to undefined function does_not_exist()
Call to undefined method stdClass::doesNotExist()
Call to undefined method stdClass::doesNotExist()
Call to undefined method stdClass::doesNotExist()
Call to private method Test::privateMethod() from global scope
Non-static method Test::instanceMethod() cannot be called statically
