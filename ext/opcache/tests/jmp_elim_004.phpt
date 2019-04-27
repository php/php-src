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

    $null = null;
    var_dump($null ?? 3);
    var_dump($null ?? new stdClass);
}
test();

?>
--EXPECT--
string(4) "test"
int(3)
object(stdClass)#1 (0) {
}
