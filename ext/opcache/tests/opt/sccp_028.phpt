--TEST--
SCCP 028: Don't propagate typed properties
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php

class Foo {
    public int $bar = 1;
}
function test() {
    $foo = new Foo();
    $foo->bar = "10";
    var_dump($foo->bar);
}
test();

?>
--EXPECT--
int(10)
