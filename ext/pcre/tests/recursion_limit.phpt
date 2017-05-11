--TEST--
PCRE Recursion limit
--SKIPIF--
<?php
if (@preg_match_all('/\p{N}/', '0123456789', $dummy) === false) {
	die("skip no support for \p support PCRE library");
}
?>
--INI--
pcre.jit=0
pcre.recursion_limit=2
--FILE--
<?php

var_dump(preg_match_all('/\p{Ll}(\p{L}((\p{Ll}\p{Ll})))/', 'aeiou', $dummy));
var_dump(preg_last_error() === PREG_RECURSION_LIMIT_ERROR);

var_dump(preg_match_all('/\p{Ll}\p{L}\p{Ll}\p{Ll}/', 'aeiou', $dummy));
var_dump(preg_last_error() === PREG_NO_ERROR);

?>
--EXPECTF--
Notice: preg_match_all(): PCRE error 3 in %s%erecursion_limit.php on line %d
bool(false)
bool(true)
int(1)
bool(true)
