--TEST--
JIT IDENTICAL: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($a, $b) {
	return $a === $b;
}
var_dump(foo(0, 0));
var_dump(foo(0, 1));
var_dump(foo(0, 0.0));
var_dump(foo(0.0, 0.0));
var_dump(foo(0.0, 1.0));
var_dump(foo("ab", "ab"));
var_dump(foo("ab", "cd"));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
