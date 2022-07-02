--TEST--
Test preg_match_all() function : error conditions - Backtracking limit
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
var_dump(preg_last_error_msg() === 'Backtrack limit exhausted');

var_dump(preg_match_all('/\p{Nd}/', '0123456789', $dummy));
var_dump(preg_last_error_msg() === 'No error');

?>
--EXPECT--
bool(false)
bool(true)
int(10)
bool(true)
