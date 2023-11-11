--TEST--
JIT MOD: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
class Test {
    public $prop = 0;
}
function test1($test) {
    $test[0] %= 3;
    return $test;
}
function test2($test) {
    $test->prop %= 3;
    return $test;
}
var_dump(test1([0]));
var_dump(test2(new Test));
?>
--EXPECT--
array(1) {
  [0]=>
  int(0)
}
object(Test)#1 (1) {
  ["prop"]=>
  int(0)
}
