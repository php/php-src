--TEST--
Script encoding autodetection (UTF7)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF7.inc");
?>
--EXPECT--
тест - ok
