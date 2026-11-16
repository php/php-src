--TEST--
JIT BW_OR: 002 (non-encodable AArch64 logical immediate, illegal instruction)
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
function foo($x) { return $x | 0x200000003; }
var_dump(foo(0));
?>
--EXPECT--
int(8589934595)
