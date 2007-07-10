--TEST--
Script encoding autodetection (UTF-16LE)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF16LE.inc");
?>
--EXPECT--
тест - ok
