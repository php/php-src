--TEST--
Test variations of getting typed class constant values
--FILE--
<?php

/* Use separate classes to make sure that in-place constant updates don't interfere */
class A {
    const object CONST1 = C;
}
class B {
    const array CONST1 = C;
}

define("C", new stdClass());

$rc = new ReflectionClassConstant(A::class, 'CONST1');
var_dump($rc->getValue());
echo $rc;

$rc = new ReflectionClassConstant(B::class, 'CONST1');
try {
    $rc->getValue();
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    echo $rc;
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
object(stdClass)#1 (0) {
}
Constant [ public object CONST1 ] { Object }
Cannot assign stdClass to class constant B::CONST1 of type array
Cannot assign stdClass to class constant B::CONST1 of type array
