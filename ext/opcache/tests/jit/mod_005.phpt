--TEST--
JIT MOD: 005
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
class Test{
    public $prop = 32;
}

function test2($test) {
    $test->prop %= 3;
	return $test;
}

var_dump(test2(new Test));
?>
--EXPECT--
object(Test)#1 (1) {
  ["prop"]=>
  int(2)
}
