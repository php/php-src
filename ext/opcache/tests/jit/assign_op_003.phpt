--TEST--
JIT ASSIGN_OP: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
    $a = 0;
    for ($i = 0; $i < 10; $i++) {
        $a += $a;
        $a =& $x;
        $a += $a;
    }
}
test();
?>
DONE
--EXPECT--
DONE
