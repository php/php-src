--TEST--
UCS4BE to ASCII
--SKIPIF--
<?php
/* include('skipif.inc'); */
if (@iconv("ascii","UCS-4LE", "abcd") == '') {
	die("skip conversion to UCS-4LE not supported"); 
}
?>
--INI--
error_reporting=2039
--FILE--
<?php
/* include('test.inc'); */
/*
Expected output:
&#97;&#98;&#99;&#100;
abcd
*/

   $s = unpack("V*", iconv("ascii","UCS-4LE", "abcd"));
   foreach($s as $c) { print "&#$c;"; } print "\n";

   $s = pack("NNNN", 97, 98, 99, 100);
   $q = iconv("UCS-4BE", "ascii", $s);
   print $q; print "\n";
?>
--EXPECT--
&#97;&#98;&#99;&#100;
abcd
