--TEST--
JIT ASSIGN_OP: 006 concationation with itself
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test($a) {
    for ($i = 0; $i < 2; $i++) {
        $a .= $a = $a;
    }
}
test("");
?>
DONE
--EXPECT--
DONE
