--TEST--
JIT MUL: 006 incorrect guard elimination 
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
for ($i = 0; $i<6; $i++) {
    $a + -$a=&$a;
    $a = 3*$a + 0xff000;
    $a += $a;
}
?>
DONE
--EXPECT--
DONE