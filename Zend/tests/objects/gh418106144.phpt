--TEST--
OSS-Fuzz #418106144
--FILE--
<?php

class Foo {
    function __toString(){}
}
function test($y=new Foo>''){
    var_dump();
}
try {
    test();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Foo::__toString(): Return value must be of type string, none returned
