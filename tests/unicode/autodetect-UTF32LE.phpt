--TEST--
Script encoding autodetection (UTF-32LE)
--INI--
unicode.semantics=on
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF32LE.inc");
?>
--EXPECT--
тест - ok
