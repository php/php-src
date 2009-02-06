--TEST--
Bug #45239 (encoding detector hangs with mbstring.strict_detection enabled)
--INI--
mbstring.strict_detection=1
mbstring.http_input=UTF-8
mbstring.internal_encoding=UTF-8
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
?>
--FILE--
<?php
mb_internal_encoding("utf-8");
mb_parse_str("a=%fc", $dummy);
var_dump(mb_http_input());
?>
--EXPECT--
string(5) "UTF-8"
