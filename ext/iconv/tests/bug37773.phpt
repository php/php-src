--TEST--
Bug #37773 (iconv_substr() gives "Unknown error" when string length = 1")
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
	var_dump(iconv_substr('x', 0, 1, 'UTF-8'));
?>
--EXPECT--
string(1) "x"