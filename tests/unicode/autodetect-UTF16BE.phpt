--TEST--
Script encoding autodetection (UTF-16BE)
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF16BE.inc");
?>
--EXPECT--
тест - ok
