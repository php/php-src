--TEST--
Dynamic class names in class constants in constant expressions
--FILE--
<?php

class A {
    const CONST = 'CONST';
}

const A_CONST = 'A';

class B {
    const CC_STRING_NAME = (self::B_NAME)::CONST;
    const B_NAME = 'B';
    const CONST = 'CONST';
    const CC_OBJECT_NAME = (A_CONST)::CONST;
}

var_dump(B::CC_STRING_NAME);
var_dump(B::CC_OBJECT_NAME);

?>
--EXPECT--
string(5) "CONST"
string(5) "CONST"
