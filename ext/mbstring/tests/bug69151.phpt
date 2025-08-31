--TEST--
Bug #69151 (mb_ereg should reject ill-formed byte sequence)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
$str = "\x80";

var_dump(false === mb_eregi('.', $str, $matches));
var_dump([] === $matches);

var_dump(NULL === mb_ereg_replace('.', "\\0", $str));

var_dump(false === mb_ereg_search_init("\x80", '.'));
var_dump(false === mb_ereg_search());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
