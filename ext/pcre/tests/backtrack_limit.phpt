--TEST--
Backtracking limit
--SKIPIF--
<?php
if (@preg_match_all('/\p{N}/', '0123456789', $dummy) === false) {
	die("skip no support for \p support PCRE library");
}
?>
--INI--
pcre.backtrack_limit=2
pcre.jit=0
--FILE--
<?php

var_dump(preg_match_all('/.*\p{N}/', '0123456789', $dummy));
var_dump(preg_last_error() === PREG_BACKTRACK_LIMIT_ERROR);

var_dump(preg_match_all('/\p{Nd}/', '0123456789', $dummy));
var_dump(preg_last_error() === PREG_NO_ERROR);

?>
--EXPECTF--
Notice: preg_match_all(): PCRE error 2 in %s%ebacktrack_limit.php on line %d
bool(false)
bool(true)
int(10)
bool(true)
