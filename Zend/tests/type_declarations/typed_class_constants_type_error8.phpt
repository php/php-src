--TEST--
Typed class constants (type mismatch; runtime)
--FILE--
<?php
class A {
    public const stdClass&Stringable CONST1 = C;
    public const stdClass&Stringable CONST2 = A::CONST1;
}

define("C", new stdClass);

try {
    var_dump(A::CONST2);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(A::CONST2);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(A::CONST1);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(A::CONST1);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Cannot assign stdClass to class constant A::CONST1 of type stdClass&Stringable
Cannot assign stdClass to class constant A::CONST1 of type stdClass&Stringable
Cannot assign stdClass to class constant A::CONST1 of type stdClass&Stringable
Cannot assign stdClass to class constant A::CONST1 of type stdClass&Stringable
