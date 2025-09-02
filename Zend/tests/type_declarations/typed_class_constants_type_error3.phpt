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
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(A::CONST1);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Cannot assign stdClass to class constant A::CONST1 of type string
Cannot assign stdClass to class constant A::CONST1 of type string
