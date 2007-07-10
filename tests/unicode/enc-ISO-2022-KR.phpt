--TEST--
Script encoding (ISO-2022-KR)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
ini_set("unicode.script_encoding", "ISO-2022-KR");
include(dirname(__FILE__)."/enc-ISO-2022-KR.inc");
?>
--EXPECT--
тест - ok
