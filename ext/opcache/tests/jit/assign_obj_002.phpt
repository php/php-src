--TEST--
JIT ASSIGN_OBJ: Assign undefined vatiable to property
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=function
--FILE--
<?php
class Test {
    public $prop;
    public int $prop2;
}
function test() {
    $o = new Test;
    $o->prop = $undef;
    var_dump($o->prop);
}
function test2() {
    $o = new Test;
    $o->prop2 = $undef;
}
test();
try {
    test2();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line %d
NULL

Warning: Undefined variable $undef in %s on line %d
Cannot assign null to property Test::$prop2 of type int
