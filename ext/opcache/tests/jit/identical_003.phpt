--TEST--
JIT IDENTICAL: 003 register allocation
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function foo(array $a, int|float $n) {
	var_dump(count($a) === $n);
}
foo([1], 1);
?>
--EXPECT--
bool(true)
