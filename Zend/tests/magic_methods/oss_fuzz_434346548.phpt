--TEST--
OSS-Fuzz #434346548: Failed assertion with throwing __toString in binary const expr
--FILE--
<?php

class Foo {
    function __toString() {}
}

function test($y = new Foo() < "") {
    var_dump();
}

try {
    test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Foo::__toString(): Return value must be of type string, none returned
