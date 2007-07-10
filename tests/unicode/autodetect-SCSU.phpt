--TEST--
Script encoding autodetection (SCSU)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-SCSU.inc");
?>
--EXPECT--
тест - ok
