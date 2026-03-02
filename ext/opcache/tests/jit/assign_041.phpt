--TEST--
JIT ASSIGN: Assign to typed reference should return modified value
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
class Test {
    public ?string $prop;
}
function test($val) {
    $obj = new Test;
    $ref =& $obj->prop;
    var_dump($ref = $val);
}
test(0);
?>
--EXPECT--
string(1) "0"