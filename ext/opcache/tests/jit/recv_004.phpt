--TEST--
JIT RECV: default arguments with type checks
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function test(int $a = 1, int $b = 2)
{
	var_dump($a, $b);
}
test(3);
?>
--EXPECT--
int(3)
int(2)
