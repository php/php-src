--TEST--
GH-24769 (mb_ereg_replace() reads past the replacement on a truncated trailing multibyte character)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
mb_regex_encoding('UTF-8');
var_dump(bin2hex(mb_ereg_replace('a', "x\xF0", 'a')));
var_dump(bin2hex(mb_ereg_replace('a', "x\xE2\x82", 'a')));
var_dump(bin2hex(mb_ereg_replace('a', "\xF0\x9F", 'bab')));
?>
--EXPECT--
string(4) "78f0"
string(6) "78e282"
string(8) "62f09f62"
