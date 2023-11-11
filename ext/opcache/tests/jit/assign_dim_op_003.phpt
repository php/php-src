--TEST--
JIT ASSIGN_DIM_OP: Undefined variable variation
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
$a = [];
$a[] &= $b;
?>
--EXTENSIONS--
opcache
--EXPECTF--
Warning: Undefined variable $b in %s on line %d
