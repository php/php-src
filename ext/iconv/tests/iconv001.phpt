--TEST--
iconv() test 1
--EXTENSIONS--
iconv
--INI--
error_reporting=2039
--FILE--
<?php
echo "iconv extension is available\n";
$test = "���";
var_dump("ISO-8859-1: $test");
var_dump("UTF-8: ".iconv( "ISO-8859-1", "UTF-8", $test ) );
?>
--EXPECT--
iconv extension is available
string(15) "ISO-8859-1: ���"
string(13) "UTF-8: æøå"
