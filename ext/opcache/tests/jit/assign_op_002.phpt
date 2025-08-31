--TEST--
JIT ASSIGN_OP: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
class Test {
    public ?string $prop = "0";
}
function test() {
    $obj = new Test;
    $ref =& $obj->prop;
    var_dump($ref &= 1);
}
test();
?>
--EXPECT--
string(1) "0"