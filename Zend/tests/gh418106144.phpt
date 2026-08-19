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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Foo::__toString(): Return value must be of type string, none returned
