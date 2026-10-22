--TEST--
Constant evaluation exception during ReflectionClass::__toString()
--FILE--
<?php

class A {
    const C = self::UNKNOWN;
}
try {
    echo new ReflectionClass(A::class);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Undefined constant self::UNKNOWN
