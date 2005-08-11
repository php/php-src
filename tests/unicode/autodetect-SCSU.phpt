--TEST--
Script encoding autodetection (SCSU)
--INI--
unicode_semantics=on
unicode.output_encoding=CP866
--FILE--
<?php
include(dirname(__FILE__)."/autodetect-SCSU.inc");
?>
--EXPECT--
тест - ok
