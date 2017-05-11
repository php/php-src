--TEST--
Test preg_match() function : error conditions - jit stacklimit exhausted
--SKIPIF--
<?php
if (ini_get("pcre.jit") === FALSE) {
	die("skip no jit built");
}
--FILE--
<?php
var_dump(preg_match('/^(foo)+$/', str_repeat('foo', 1024*8192)));
var_dump(preg_last_error() === PREG_JIT_STACKLIMIT_ERROR);
?>
--EXPECTF--
Notice: preg_match(): PCRE error 6 in %s%epreg_match_error3.php on line %d
bool(false)
bool(true)
