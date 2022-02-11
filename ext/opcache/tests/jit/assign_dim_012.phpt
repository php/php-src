--TEST--
JIT ASSIGN_DIM: 012
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test($key) {
    $ao = new ArrayObject();
    $ao[$key] = 1 < $ao['321'] = 2;
}
test('0');
?>
DONE
--EXPECTF--
DONE
