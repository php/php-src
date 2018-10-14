--TEST--
Bug #37773 (iconv_substr() gives "Unknown error" when string length = 1")
--SKIPIF--
<?php
include('skipif.inc');

$test = @iconv_strpos("abbttt","ttt",0,"UTF-8");
if ($test === false) {
	die("skip UTF-8 is not supported?");
}

?>
--FILE--
<?php
	var_dump(iconv_substr('x', 0, 1, 'UTF-8'));
?>
--EXPECT--
string(1) "x"
