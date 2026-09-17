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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    does_not_exist(...);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    stdClass::doesNotExist(...);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    (new stdClass)->doesNotExist(...);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    [new stdClass, 'doesNotExist'](...);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    Test::privateMethod(...);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    Test::instanceMethod(...);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Value of type int is not callable
Error: Call to undefined function does_not_exist()
Error: Call to undefined method stdClass::doesNotExist()
Error: Call to undefined method stdClass::doesNotExist()
Error: Call to undefined method stdClass::doesNotExist()
Error: Call to private method Test::privateMethod() from global scope
Error: Non-static method Test::instanceMethod() cannot be called statically
