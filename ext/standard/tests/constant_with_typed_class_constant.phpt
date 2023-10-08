--TEST--
Calling constant() with a typed class constant
--FILE--
<?php

class Foo {
    const object CONST1 = C;
    const array CONST2 = C;
}

define("C", new stdClass());

var_dump(constant("FOO::CONST1"));

try {
    constant("FOO::CONST2");
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
object(stdClass)#1 (0) {
}
Cannot assign stdClass to class constant Foo::CONST2 of type array
