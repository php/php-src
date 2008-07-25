--TEST--
zend multibyte (6)
--SKIPIF--
<?php
ini_set("mbstring.script_encoding", "SJIS");
ini_set("mbstring.script_encoding", "***") != "SJIS" and
	die("skip zend-multibyte is not available");
?>
--INI--
mbstring.script_encoding=EUC-JP
mbstring.internal_encoding=CP932
--FILE--
<?php
declare(encoding="UTF-8");
var_dump(bin2hex("テスト"));
?>
--EXPECT--
string(12) "836583588367"
