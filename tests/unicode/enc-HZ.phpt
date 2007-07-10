--TEST--
Script encoding (HZ)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
ini_set("unicode.script_encoding", "HZ");
include(dirname(__FILE__)."/enc-HZ.inc");
?>
--EXPECT--
тест - ok
