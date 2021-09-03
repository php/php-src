--TEST--
SCCP 029: Don't propagate assignments to references to typed properties
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php

class Test {
    public string $x = "x";
}
function test() {
    $test = new Test();
    $ref = "y";
    $test->x =& $ref;
    $ref = 42;
    var_dump($ref);
}
test();

?>
--EXPECT--
string(2) "42"
