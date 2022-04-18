--TEST--
JIT ROPE: 001 *_ROPE may types of temporary variables
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
$a = "";
$b = 1;
var_dump(" $a $a" == " $a" . -$b);
var_dump(" $a $a" == " $a" . -$b);
?>
--EXPECT--
bool(false)
bool(false)
