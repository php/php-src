--TEST--
Bug #79371 (mb_strtolower (UTF-32LE): stack-buffer-overflow)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
?>
--FILE--
<?php
$bytes = array(0xef, 0xbf, 0xbd, 0xef);
$str = implode(array_map("chr", $bytes));
var_dump(bin2hex(mb_strtolower($str, "UTF-32LE")));
?>
--EXPECT--
string(8) "3f000000"
