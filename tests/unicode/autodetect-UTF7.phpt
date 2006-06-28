--TEST--
Script encoding autodetection (UTF7)
--INI--
unicode.semantics=on
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-UTF7.inc");
?>
--EXPECT--
тест - ok
