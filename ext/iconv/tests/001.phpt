--TEST--
iconv test
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php
echo "iconv extension is available\n";
$test = "æøå";
var_dump("ISO-8859-1: $test");
var_dump("UTF-8: ".iconv( "ISO-8859-1", "UTF-8", $test ) );
?>
--EXPECT--
iconv extension is available
string(15) "ISO-8859-1: æøå"
string(13) "UTF-8: Ã¦Ã¸Ã¥"
