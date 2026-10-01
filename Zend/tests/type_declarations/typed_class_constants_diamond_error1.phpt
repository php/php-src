--TEST--
Typed class constants (diamond error with self)
--FILE--
<?php
class A {
    public const self CONST1 = C;
}

try {
    define("C", new A());
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
Error: Undefined constant "C"
