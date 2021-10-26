--TEST--
PRE_INC_OBJ: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
class Test {
    public float $prop = 1.0;
}
$test = new Test;
$r = &$test->prop;
$v = --$test->prop;
var_dump($v);
?>
--EXPECT--
float(0)