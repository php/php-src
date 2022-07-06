--TEST--
Exception thrown while converting ReflectionClassConstant to string
--FILE--
<?php

class B {
    const X = self::UNKNOWN;
}

try {
    echo new ReflectionClassConstant('B', 'X');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant self::UNKNOWN
