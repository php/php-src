--TEST--
zend multibyte (6)
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
--EXPECTF--
php: Zend/zend_language_scanner.l:%d: encoding_filter_script_to_internal: Assertion `internal_encoding && zend_multibyte_check_lexer_compatibility(internal_encoding)' failed.
