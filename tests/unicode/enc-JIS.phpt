--TEST--
Script encoding (JIS)
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
ini_set("unicode.script_encoding", "JIS");
include(dirname(__FILE__)."/enc-JIS.inc");
?>
--EXPECT--
тест - ok
