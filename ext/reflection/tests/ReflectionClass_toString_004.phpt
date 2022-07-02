--TEST--
Constant evaluation exception during ReflectionClass::__toString()
--FILE--
<?php

class A {
    const C = self::UNKNOWN;
}
try {
    echo new ReflectionClass(A::class);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant self::UNKNOWN
