--TEST--
JIT ASSIGN_DIM: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=tracing
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test() {
    var_dump($a[0] = $v);
}
test();
?>
--EXPECTF--
Warning: Undefined variable $v in %sassign_dim_003.php on line 3
NULL