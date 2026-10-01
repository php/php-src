--TEST--
Typed class constants (type mismatch; runtime simple)
--FILE--
<?php
class A {
    public const int CONST1 = C;
}

define("C", "c");

try {
    var_dump(A::CONST1);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    var_dump(A::CONST1);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot assign string to class constant A::CONST1 of type int
TypeError: Cannot assign string to class constant A::CONST1 of type int
