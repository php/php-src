--TEST--
Typed class constants (type mismatch; runtime object)
--FILE--
<?php
class A {
    public const string CONST1 = C;
}

define('C', new stdClass);

try {
    var_dump(A::CONST1);
} catch (TypeError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    var_dump(A::CONST1);
} catch (TypeError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Cannot assign stdClass to class constant A::CONST1 of type string
TypeError: Cannot assign stdClass to class constant A::CONST1 of type string
