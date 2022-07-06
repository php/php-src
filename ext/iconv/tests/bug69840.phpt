--TEST--
Bug #69840 (iconv_substr() doesn't work with UTF-16BE)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
$str = iconv_substr("a\x00b\x00", 0, 1, 'UTF-16LE');
var_dump(strlen($str));
var_dump(ord($str[0]));
var_dump(ord($str[1]));
$str = iconv_substr("\x00a\x00b", 0, 1, 'UTF-16BE');
var_dump(strlen($str));
var_dump(ord($str[0]));
var_dump(ord($str[1]));
?>
--EXPECT--
int(2)
int(97)
int(0)
int(2)
int(0)
int(97)
