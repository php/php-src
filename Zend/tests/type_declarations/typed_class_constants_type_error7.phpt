--TEST--
Typed class constants (type mismatch; runtime)
--FILE--
<?php
class A {
    public const stdClass&Stringable CONST1 = C;
}

define("C", new stdClass);

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
TypeError: Cannot assign stdClass to class constant A::CONST1 of type stdClass&Stringable
TypeError: Cannot assign stdClass to class constant A::CONST1 of type stdClass&Stringable
