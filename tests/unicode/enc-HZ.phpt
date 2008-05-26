--TEST--
Script encoding (HZ)
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
ini_set("unicode.script_encoding", "HZ");
include(dirname(__FILE__)."/enc-HZ.inc");
?>
--EXPECT--
тест - ok
