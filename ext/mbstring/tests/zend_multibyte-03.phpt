--TEST--
zend multibyte (3)
--SKIP--
<?php
ini_set("mbstring.script_encoding", "SJIS");
ini_set("mbstring.script_encoding", "***") != "SJIS" or
	die("skip zend-multibyte is not available");
?>
--INI--
mbstring.script_encoding=UTF-8
mbstring.internal_encoding=EUC-JP
--FILE--
<?php
var_dump(bin2hex("テスト"));
?>
--EXPECT--
string(12) "a5c6a5b9a5c8"
