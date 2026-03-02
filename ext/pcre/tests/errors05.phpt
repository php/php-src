--TEST--
Test preg_match() function : error conditions - jit stacklimit exhausted
--SKIPIF--
<?php
if (ini_get('pcre.jit') === false) {
    die("skip no jit built");
}
?>
--INI--
pcre.jit=1
--FILE--
<?php
var_dump(preg_match('/^(foo)+$/', str_repeat('foo', 1024*8192)));
var_dump(preg_last_error_msg() === 'JIT stack limit exhausted');
?>
--EXPECT--
bool(false)
bool(true)
