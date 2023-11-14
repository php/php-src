--TEST--
JIT CMP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
$a = 0;
$b = 0.0;
var_dump($a < $b ? 1 : 0);
var_dump($a > $b ? 1 : 0);
var_dump($a <= $b ? 1 : 0);
var_dump($a >= $b ? 1 : 0);
?>
--EXPECT--
int(0)
int(0)
int(1)
int(1)
