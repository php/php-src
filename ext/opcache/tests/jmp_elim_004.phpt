--TEST--
Incorrect empty basic block elimination
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function test() {
    $foo = "test";
    var_dump($foo ?? "default");
}
test();

?>
--EXPECT--
string(4) "test"
