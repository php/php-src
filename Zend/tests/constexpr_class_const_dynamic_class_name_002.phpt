--TEST--
Dynamic class names in class constants in constant expressions
--FILE--
<?php

class B {
    const CC = (self::C_NAME)::CONST;
    const C_NAME = [];
}

try {
    var_dump(B::CC);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Class name must be a valid object or a string
