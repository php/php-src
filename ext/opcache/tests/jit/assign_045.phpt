--TEST--
JIT ASSIGN: incorrect assumption about in-memeory zval type
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test($x, $y) {
    $a = $b = $a = $y;
    var_dump($a + $x);
}
test(null, 1);
?>
--EXPECT--
int(1)

