--TEST--
Script encoding autodetection (UTF8)
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF8.inc");
?>
--EXPECT--
тест - ok
