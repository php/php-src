--TEST--
JIT ASSIGN_DIM: 017
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
    for($i = 0; $i < 10; $i++) {
        $a[] = 0;
        $a = false;
    }
}
@test();
?>
DONE
--EXPECT--
DONE