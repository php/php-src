--TEST--
Script encoding autodetection (UTF-16LE)
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF16LE.inc");
?>
--EXPECT--
тест - ok
