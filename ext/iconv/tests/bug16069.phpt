--TEST--
Bug #16069 (ICONV transliteration failure)
--SKIPIF--
<?php
include( 'skipif.inc' );
if (@iconv('CP932', 'EUC-JP//TRANSLIT', "\x87\x6d")=='') {
	die("skip  CP932 to EUC-JP translit not available\n");
}
?>
--INI--
error_reporting=2039
--FILE--
<?php
/* include('test.inc'); */
/* charset=CP932, KOI8-R */
$str = "\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d\x87\x6d";
print iconv( "CP932", "EUC-JP//TRANSLIT", $str );
$str = "(\x9c\xe4\x9c)";
print iconv( "KOI8-R", "EUC-JP//TRANSLIT", $str );
?>
--EXPECT--
ミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバール(°Д°)
