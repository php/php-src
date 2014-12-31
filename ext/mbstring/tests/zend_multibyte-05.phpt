--TEST--
zend multibyte (5)
--SKIPIF--
--INI--
zend.multibyte=On
zend.script_encoding=EUC-JP
mbstring.internal_encoding=UTF-8
--FILE--
<?php
// forcefully interpret an UTF-8 encoded string as EUC-JP and then convert it
// back to UTF-8. There should be only a pair of consecutive bytes that is
// valid EUC-encoded character "鴻".
var_dump(bin2hex("テスト"));
?>
--EXPECT--
string(16) "3f3f3fe9b4bb3f3f"
