--TEST--
zend multibyte (6)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
zend.multibyte=On
zend.script_encoding=EUC-JP
internal_encoding=CP932
--FILE--
<?php
declare(encoding="UTF-8");
var_dump(bin2hex("テスト"));
?>
--EXPECT--
string(12) "836583588367"
