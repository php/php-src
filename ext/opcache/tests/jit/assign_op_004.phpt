--TEST--
JIT ASSIGN_OP: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
    $a =& $v;
    $a = 0;
    $b = 0; 
    for ($i = 0; $i < 10; $i++) {
        $a *= 64;
        $b += $a;
        $a += $b + $a;
        $a++;
    }
}
test(); 
?>
DONE
--EXPECT--
DONE
