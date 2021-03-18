--TEST--
Exception thrown while converting ReflectionClassConstant to string
--FILE--
<?php

try {
    class B {
        const X = self::UNKNOWN;
    }
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo new ReflectionClassConstant('B', 'X');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant self::UNKNOWN
Undefined constant self::UNKNOWN
