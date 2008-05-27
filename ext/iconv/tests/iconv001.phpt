--TEST--
iconv() test 1
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
unicode.script_encoding=ISO-8859-1
unicode.output_encoding=ISO-8859-1
error_reporting=2039
--FILE--
<?php
echo "iconv extension is available\n";
$test = "æøå";
var_dump("ISO-8859-1: $test");
var_dump("UTF-8: ".iconv( "ISO-8859-1", "UTF-8", $test ) );
?>
--EXPECT--
iconv extension is available
unicode(15) "ISO-8859-1: æøå"
unicode(13) "UTF-8: Ã¦Ã¸Ã¥"
