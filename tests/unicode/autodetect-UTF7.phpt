--TEST--
Script encoding autodetection (UTF7)
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF7.inc");
?>
--EXPECT--
тест - ok
