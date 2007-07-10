--TEST--
Script encoding autodetection (UTF-32BE)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF32BE.inc");
?>
--EXPECT--
тест - ok
