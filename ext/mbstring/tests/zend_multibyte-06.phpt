--TEST--
zend multibyte (6)
--SKIPIF--
--XFAIL--
https://bugs.php.net/bug.php?id=66582
--INI--
zend.multibyte=On
zend.script_encoding=EUC-JP
mbstring.internal_encoding=CP932
--FILE--
<?php
declare(encoding="UTF-8");
var_dump(bin2hex("テスト"));
?>
--EXPECT--
string(12) "836583588367"
