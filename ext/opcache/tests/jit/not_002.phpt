--TEST--
JIT NOT: 001
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
function foo(float $x) {
	return !$x;
}
var_dump(foo(1.0));
var_dump(foo(0.0));
?>
--EXPECT--
bool(false)
bool(true)
