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
--FILE--
<?php

var_dump(preg_match_all('/.*\p{N}/', '0123456789', $dummy));
var_dump(preg_last_error() === PREG_BACKTRACK_LIMIT_ERROR);

var_dump(preg_match_all('/\p{Nd}/', '0123456789', $dummy));
var_dump(preg_last_error() === PREG_NO_ERROR);

?>
--EXPECT--
int(0)
bool(true)
int(10)
bool(true)
