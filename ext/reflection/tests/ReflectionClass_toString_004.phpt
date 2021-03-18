--TEST--
Constant evaluation exception during ReflectionClass::__toString()
--FILE--
<?php

try {
    class A {
        const C = self::UNKNOWN;
    }
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo new ReflectionClass(A::class);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant self::UNKNOWN
Undefined constant self::UNKNOWN
