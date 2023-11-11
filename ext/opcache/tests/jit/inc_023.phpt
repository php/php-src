--TEST--
PRE_INC/DEC refcounted typed property
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--FILE--
<?php
class Test {
    public string $prop;
}

$test = new Test;
$test->prop = "a";
++$test->prop;
--$test->prop;
var_dump(++$test->prop);
var_dump(--$test->prop);
?>
--EXPECT--
string(1) "c"
string(1) "c"
