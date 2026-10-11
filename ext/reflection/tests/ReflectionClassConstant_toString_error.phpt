--TEST--
Exception thrown while converting ReflectionClassConstant to string
--FILE--
<?php

class B {
    const X = self::UNKNOWN;
}

try {
    echo new ReflectionClassConstant('B', 'X');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Undefined constant self::UNKNOWN
