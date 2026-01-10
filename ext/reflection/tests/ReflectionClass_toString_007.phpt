--TEST--
Using ReflectionClass::__toString() with typed class constants when there is a type mismatch
--FILE--
<?php

class Foo {
    const array CONST1 = C;
}

define("C", new stdClass());

try {
    (string) new ReflectionClass(Foo::class);
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Cannot assign stdClass to class constant Foo::CONST1 of type array
