--TEST--
zend multibyte (3)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
zend.multibyte=On
zend.script_encoding=UTF-8
internal_encoding=EUC-JP
--FILE--
<?php
var_dump(bin2hex("テスト"));
?>
--EXPECT--
string(12) "a5c6a5b9a5c8"
