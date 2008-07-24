--TEST--
zend multibyte (2)
--SKIP--
<?php
ini_set("mbstring.script_encoding", "SJIS");
ini_set("mbstring.script_encoding", "***") != "SJIS" or
	die("skip zend-multibyte is not available");
?>
--INI--
mbstring.script_encoding=UTF-8
mbstring.internal_encoding=CP932
--FILE--
<?php
var_dump(bin2hex("テスト"));
?>
--EXPECT--
string(12) "836583588367"
