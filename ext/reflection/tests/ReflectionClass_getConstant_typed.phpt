--TEST--
ReflectionClass::getConstant() with typed class constants
--FILE--
<?php
class C {
    const object CONST1 = C;
}

class D {
    const array CONST1 = C;
}

const C = new stdClass();

$rc = new ReflectionClass(C::class);
var_dump($rc->getConstant("CONST1"));

$rc = new ReflectionClass(D::class);
try {
    $rc->getConstant("CONST1");
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
object(stdClass)#1 (0) {
}
Cannot assign stdClass to class constant D::CONST1 of type array
