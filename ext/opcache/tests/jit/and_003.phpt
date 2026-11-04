--TEST--
JIT BW_AND: 003 (non-encodable AArch64 logical immediate, wrong result)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
opcache.jit_buffer_size=64M
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
function foo($x) { return $x & 0x5555555555555554; }
var_dump(foo(-1));
?>
--EXPECT--
int(6148914691236517204)
