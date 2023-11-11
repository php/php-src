--TEST--
JIT FETCH_DIM_W: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function test() {
    $b = [&$a[0]];
}
test();
?>
DONE
--EXPECT--
DONE
